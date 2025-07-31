#pragma once
#include "imgui.h"
#include "imgui_internal.h"

#ifndef IMGUI_DISABLE

#include <cctype>
#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <limits>
#include <memory>
#include <numeric>
#include <stack>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace ImSearch {
using IndexT = std::uint32_t;

//-----------------------------------------------------------------------------
// [SECTION] 常量
//-----------------------------------------------------------------------------

// 凡是低于这个分数的都不会显示给用户。
// 将来可以在运行时调整
constexpr float sCutOffStrength = .5f;

constexpr IndexT sNullIndex = std::numeric_limits<IndexT>::max();

//-----------------------------------------------------------------------------
// [SECTION] 结构体
//-----------------------------------------------------------------------------

struct StrView {
  StrView() = default;
  StrView(const std::string &str)
      : mData(str.data()), mSize(static_cast<IndexT>(str.size())) {}
  StrView(const char *data, IndexT size) : mData(data), mSize(size) {}

  const char *begin() const { return mData; }
  const char *end() const { return mData + mSize; }
  const char *data() const { return mData; }
  IndexT size() const { return mSize; }

  const char &operator[](IndexT i) const {
    IM_ASSERT(i < mSize);
    return mData[i];
  }

  const char *mData{};
  IndexT mSize{};
};

// 这里我们使用VTable而不是std::function的原因：
// std::function的SBO优化意味着如果用户没有捕获任何东西，
// 它可能会消耗比需要的更多的内存，
// 如果用户捕获的内容超过了SBO存储的大小，
// 性能会因大量的堆分配而下降。现在，
// 使用Callback比使用std::function慢，
// 它使得实现和API稍微复杂一些，但是，如果我们
// 在公共API中使用std::function发布，我将来
// 就无法在不破坏向前兼容性的情况下摆脱std::function。
// 通过在后台保持如何存储functor的概念抽象化，
// 可以在不破坏API的情况下优化它。
struct Callback {
  Callback() = default;

  Callback(void *originalFunctor, ImSearch::Internal::VTable vTable);

  Callback(const Callback &) = delete;
  Callback(Callback &&other) noexcept;

  Callback &operator=(const Callback &) = delete;
  Callback &operator=(Callback &&other) noexcept;

  ~Callback();

  operator bool() const { return mUserFunctor != nullptr; }

  // PushSearchable
  bool operator()(const char *name) const;

  // PopSearchable
  void operator()() const;

  static bool InvokeAsPushSearchable(ImSearch::Internal::VTable vTable,
                                     void *userFunctor, const char *name);
  static void InvokeAsPopSearchable(ImSearch::Internal::VTable vTable,
                                    void *userFunctor);

  void ClearData();

  enum VTableModes { Invoke = 0, MoveConstruct = 1, Destruct = 2, GetSize = 3 };

  Internal::VTable mVTable{};
  void *mUserFunctor{};
};

struct Searchable {
  std::string mText{};

  IndexT mIndexOfFirstChild = sNullIndex;
  IndexT mIndexOfLastChild = sNullIndex;
  IndexT mIndexOfParent = sNullIndex;
  IndexT mIndexOfNextSibling = sNullIndex;
};

struct Input {
  ImSearchFlags mFlags{};
  std::vector<Searchable> mEntries{};
  std::vector<float> mBonuses{};
  std::string mUserQuery{};
};

struct ReusableBuffers {
  std::vector<float> mScores{};
  std::vector<IndexT> mTempIndices{};
};

struct Output {
  std::vector<IndexT> mDisplayOrder{};
  static constexpr IndexT sDisplayEndFlag =
      static_cast<IndexT>(1)
      << static_cast<IndexT>(std::numeric_limits<IndexT>::digits - 1);

  std::string mPreviewText{};
};

struct Result {
  Input mInput{};
  ReusableBuffers mBuffers{};
  Output mOutput{};
};

struct DisplayCallbacks {
  Callback mOnDisplayStart{};
  Callback mOnDisplayEnd{};
};

struct LocalContext {
  Input mInput{};

  std::vector<DisplayCallbacks> mDisplayCallbacks{};
  std::stack<IndexT> mPushStack{};

  // Used for debugging. We don't need
  // to keep track of the actual stack
  // when the user is not actively searching,
  // so we only track the size to detect
  // push/pop underflows and overflows.
  int mPushStackLevel{};

  Result mResult{};
  bool mHasSubmitted{};
};

struct ImSearchContext {
  std::unordered_map<ImGuiID, LocalContext> Contexts{};
  std::stack<std::reference_wrapper<LocalContext>> ContextStack{};
  std::unordered_map<std::string, std::string> mTokenisedStrings{};

  // Style and Colormaps
  ImSearchStyle Style;
  ImVector<ImGuiColorMod> ColorModifiers;
};

bool operator==(const StrView &lhs, const StrView &rhs);
bool operator==(const Searchable &lhs, const Searchable &rhs);
bool operator==(const Input &lhs, const Input &rhs);

//-----------------------------------------------------------------------------
// [SECTION] Context
//-----------------------------------------------------------------------------

// Some helper functions for generalising error-reporting.
LocalContext &GetLocalContext();
ImSearch::ImSearchContext &GetImSearchContext();
IndexT GetCurrentItem(LocalContext &context);

// Is the context currently collecting submissions?
// ImSearch does not store anything the programm is submitting if the user
// is not actively searching, for performance and memory reasons.
bool CanCollectSubmissions();

//-----------------------------------------------------------------------------
// [SECTION] Testing
//-----------------------------------------------------------------------------

float GetScore(size_t index);

size_t GetDisplayOrderEntry(size_t index);

//-----------------------------------------------------------------------------
// [SECTION] Future API candidates
//-----------------------------------------------------------------------------

int GetNumItemsFilteredOut();

void SetPreviewText(const char *preview);

const char *GetPreviewText();

void BeginHighlightZone(const char *textToHighlight);

void EndHighlightZone();

//-----------------------------------------------------------------------------
// [SECTION] Fuzzy Searching & String Functions
//-----------------------------------------------------------------------------

std::vector<std::string> SplitTokens(StrView s);

std::string Join(const std::vector<std::string> &tokens);

StrView GetStringNeededToCompletePartial(StrView partial, StrView complete);

std::string MakeTokenisedString(StrView original);

StrView GetMemoizedTokenisedString(const std::string &original);

IndexT LevenshteinDistance(StrView s1, StrView s2, ReusableBuffers &buffers);

float Ratio(StrView s1, StrView s2, ReusableBuffers &buffers);

float PartialRatio(StrView s1, StrView s2, ReusableBuffers &buffers);

// The function used internally to score strings
float WeightedRatio(StrView s1, StrView s1Tokenised, StrView s2,
                    StrView s2Tokenised, ReusableBuffers &buffers);
} // namespace ImSearch

#endif // #ifndef IMGUI_DISABLE
