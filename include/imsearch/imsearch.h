#pragma once
#include "imgui.h"
#ifndef IMGUI_DISABLE

typedef int ImSearchCol; // -> enum ImSearchCol_
typedef int
    ImSearchFlags; // -> enum ImSearchFlags_     // Flags: for BeginSearch()

enum ImSearchFlags_ {
  ImSearchFlags_None = 0,
  ImSearchFlags_NoTextHighlighting = 1 << 0
};

// 绘图样式颜色。
enum ImSearchCol_ {
  // 项目样式颜色
  ImSearchCol_TextHighlighted, // 所有匹配的子字符串默认都会被高亮显示，并且将使用这种颜色
  ImSearchCol_TextHighlightedBg, // 所有高亮文本的背景颜色
  ImSearchCol_COUNT
};

// 绘图样式结构
struct ImSearchStyle {
  // 样式颜色
  ImVec4 Colors
      [ImSearchCol_COUNT]; // 风格颜色数组。可以使用ImSearchCol_枚举进行索引。

  ImSearchStyle();
};

namespace ImSearch {
//-----------------------------------------------------------------------------
// [部分] 上下文
//-----------------------------------------------------------------------------

struct ImSearchContext;

// 创建一个新的ImSearch上下文。在调用ImGui::CreateContext之后调用此函数。
ImSearchContext *CreateContext();

// 销毁一个ImSearch上下文。在调用ImGui::DestroyContext之前调用此函数。nullptr =
// 销毁当前上下文
void DestroyContext(ImSearchContext *ctx = nullptr);

// 返回当前上下文。如果没有设置上下文，则返回nullptr。
ImSearchContext *GetCurrentContext();

// 设置当前上下文。
void SetCurrentContext(ImSearchContext *ctx);

//-----------------------------------------------------------------------------
// [SECTION] 开始/结束搜索上下文
//-----------------------------------------------------------------------------

// 开始一个搜索上下文。如果此函数返回true，则MUST调用EndSearch()！建议使用以下约定：
//
//	if (BeginSearch())
//	{
//		SearchBar();
//
//		ImSearch::SearchableItem("Hello world!",
//			[](const char* str)
//			{
//				ImGui::Button(str);
//			});
//
//		...
//
//		EndSearch();
//	}
//
// 参见imsearch_demo.cpp以获取更多示例。
//
// 重要注意事项：
//
// - BeginSearch必须唯一地位于当前的ImGui
// ID范围内，多次调用BeginSearch会导致ID冲突。如果需要避免ID
//   冲突，请使用ImGui::PushId
bool BeginSearch(ImSearchFlags flags = 0);

// 只有在BeginSearch()返回true的情况下才调用EndSearch()！通常在if语句的末尾调用，
// 该语句的条件是BeginSearch()。参见上面的示例。
// 如果用户尚未显式调用Submit，则调用Submit。
// 参见下面的Submit文档。
void EndSearch();

// ImSearch可以在您提交它们和调用EndSearch之间的任何地方调用您的回调函数。
// 有时您需要更多控制ImSearch调用回调函数的时间，这就是Submit的用途。
//
// 例如，当您希望回调函数在ImGui子窗口中调用时：
//
//	if (ImSearch::BeginSearch())
//	{
//		ImSearch::SearchBar();
//
//		if (ImGui::BeginChild("Submissions", {},
// ImGuiChildFlags_Borders))
//		{
//			ImSearch::SearchableItem("Hello world!",
//				[](const char* str)
//				{
//					ImGui::Button(str);
//				});
//
//			// 显式调用Submit；所有回调
//			// 将通过submit调用。如果我们
//			// 等待EndSearch为我们做这件事，
//			// 回调将在ImGui::EndChild之后调用，
//			// 这会使我们的搜索项显示在子窗口之外。
//			ImSearch::Submit();
//		} ImGui::EndChild();
//
//		ImSearch::EndSearch();
//	}
void Submit();

//-----------------------------------------------------------------------------
// [SECTION] 提交搜索项
//-----------------------------------------------------------------------------

// 添加一个带有回调的搜索项，例如包装一些ImGui函数调用：
//
//	ImSearch::SearchableItem("Hello world!",
//		[](const char* str)
//		{
//			ImGui::Button(str);
//		});
//
// 请注意，回调函数按照相关性顺序调用，或者可能根本不会调用，
// 如果它们不相关。ImSearch可以在您提交它们和调用EndSearch或Submit之间的任何地方调用它们。
// 回调可能有与之关联的数据（例如，lambda捕获）。
// 确保您的回调对象在那时不会引用任何超出范围的内容。
// 有关更多信息，请参见imsearch_demo.cpp中的“回调是如何工作的？”。
//
// 回调是一个对象或函数指针，具有以下形式的函数：void Func(const char* name)。
template <typename T> void SearchableItem(const char *name, T &&callback);

// 推送一个带有回调的搜索项。
// 如果此函数返回true，则MUST调用EndSearch()
// 您建议使用以下约定：
//
// if (PushSearchable("Hello world!", [](const char* name) { return
// ImGui::TreeNode(name); })
// {
//		PopSearchable([](){ ImGui::TreePop(); });
// }
//
// 提供的回调函数应返回true，如果“子项”也应该
// 显示，类似于ImGui的树节点。
//
// 回调是一个对象或函数指针，具有以下形式的函数：bool Func(const char* name)。
//
// 请注意，回调函数按照相关性顺序调用，或者可能根本不会调用，
// 如果它们不相关。ImSearch可以在您提交它们和调用EndSearch或Submit之间的任何地方调用它们。
// 回调可能有与之关联的数据（例如，lambda捕获）。
// 确保您的回调对象在那时不会引用任何超出范围的内容。
// 有关更多信息，请参见imsearch_demo.cpp中的“回调是如何工作的？”。
template <typename T> bool PushSearchable(const char *name, T &&callback);

// 仅在 PushSearchable() 返回 true 时调用 PopSearchable()！通常在 if
// 语句的末尾调用， 该语句的条件是 PushSearchable()。参见上面的 PushSearchable
// 示例。
void PopSearchable();

// 仅在 PushSearchable() 返回 true 时调用 PopSearchable()！通常在 if
// 语句的末尾调用， 该语句的条件是 PushSearchable()。
// 您可以使用回调来包装所有结束小部件所需的 ImGui 调用，
// 一个常见的例子是 `ImGui::TreePop()`，参见上面的 PushSearchable 示例。
//
// 回调是一个对象或函数指针，具有以下形式的函数：void Func()。
template <typename T> void PopSearchable(T &&callback);

//-----------------------------------------------------------------------------
// [SECTION] Modifiers
//-----------------------------------------------------------------------------

// 您可以人为地增加您认为用户更有可能寻找的项的相关性。
//
// 例如，您可以优先考虑更常引用的函数：
//
//	if (ImSearch::PushSearchable(func.name, &DisplayFuncWidget))
//	{
//		float frequency = func.timesUsedInCodebase /
// gTotalFunctionsUsedInCodeBase;
// ImSearch::SetRelevancyBonus(frequency);
// ImSearch::PopSearchable();
//	}
//
// ImSearch 对您提供的奖励范围没有限制，
// 但请记住，ImSearch 评分的项的“默认”相关性范围在 0.0f 到 1.0f 之间。
// 鼓励归一化，以避免您的奖励 dwarfing 文本相似度得分。
void SetRelevancyBonus(float bonus);

// 您可以使用以下语法添加同义词：
//
//	if (ImSearch::PushSearchable("Function", selectableCallback))
//	{
//		ImSearch::AddSynonym("Method");
//		ImSearch::AddSynonym("Procedure");
//
//		ImSearch::PopSearchable();
//	}
//
void AddSynonym(const char *synonym);

//-----------------------------------------------------------------------------
// [SECTION] Searchbars
//-----------------------------------------------------------------------------

// 显示默认的搜索栏。通常放在 BeginSearch 之后，或者在调用 Submit 之后。
void SearchBar(const char *hint = "Search");

// 用于设置用户查询的API，即用户当前输入的文本
// 并正在搜索。用于制作自定义搜索栏。
void SetUserQuery(const char *query);

// 将返回用户当前输入的文本
// 并正在搜索。
const char *GetUserQuery();

//-----------------------------------------------------------------------------
// [SECTION] Styling
//-----------------------------------------------------------------------------

// 与 ImGui 类似，所有样式颜色都存储在 ImSearchStyle 的可索引数组中。
// 您可以通过 GetStyle().Colors 永久修改这些值，
// 或者使用下面的 Push/Pop 函数临时修改它们。

ImSearchStyle &GetStyle();

ImU32 GetColorU32(ImSearchCol idx, float alpha_mul = 1.0f);
const ImVec4 &GetStyleColorVec4(ImSearchCol idx);

// 使用PushStyleX临时修改您的ImSearchStyle。修改将持续到匹配的PopStyleX调用为止。您必须为每个push调用pop，否则会导致内存泄漏！这与ImGui的行为相同。

// 临时修改一个样式颜色。别忘了调用PopStyleColor！
void PushStyleColor(ImSearchCol idx, ImU32 col);
void PushStyleColor(ImSearchCol idx, const ImVec4 &col);

// 撤销临时样式颜色修改。通过增加计数一次撤销多个推送。
void PopStyleColor(int count = 1);

//-----------------------------------------------------------------------------
// [SECTION] 演示
//-----------------------------------------------------------------------------

// 显示ImSearch演示窗口（将imsearch_demo.cpp添加到您的源文件中！）
void ShowDemoWindow(bool *p_open = nullptr);

//-----------------------------------------------------------------------------
// [SECTION] 内部
//-----------------------------------------------------------------------------

// 公共API的结束！
// 从这里开始直到文件结束，
// 不保证向前兼容！

namespace Internal {
using VTable = bool (*)(int mode, void *ptr1, void *ptr2);

bool PushSearchable(const char *name, void *callback, VTable vTable);
void PopSearchable(void *callback, VTable vTable);

template <class T> struct remove_reference {
  typedef T type;
};
template <class T> struct remove_reference<T &> {
  typedef T type;
};
template <class T> struct remove_reference<T &&> {
  typedef T type;
};
} // namespace Internal
} // namespace ImSearch

template <typename T>
void ImSearch::SearchableItem(const char *name, T &&callback) {
  using TNonRef = typename Internal::remove_reference<T>::type;

  struct CallbackWrapper {
    TNonRef mUserCallback;

    bool operator()(const char *name) const {
      (void)mUserCallback(name);
      return false;
    }
  };

  if (PushSearchable(
          name, CallbackWrapper{static_cast<decltype(callback)>(callback)})) {
    PopSearchable();
  }
}

template <typename T>
bool ImSearch::PushSearchable(const char *name, T &&callback) {
  using TNonRef = typename Internal::remove_reference<T>::type;
  TNonRef moveable{static_cast<decltype(callback)>(callback)};
  return Internal::PushSearchable(
      name, &moveable, +[](int mode, void *ptr1, void *ptr2) -> bool {
        switch (mode) {
        case 0: // Invoke
        {
          TNonRef *func = static_cast<TNonRef *>(ptr1);
          const char *nameArg = static_cast<const char *>(ptr2);
          return (*func)(nameArg);
        }
        case 1: // Move-construct
        {
          TNonRef *src = static_cast<TNonRef *>(ptr1);
          TNonRef *dst = static_cast<TNonRef *>(ptr2);
          new (dst) TNonRef(static_cast<TNonRef &&>(*src));
          return true;
        }
        case 2: // Destructor
        {
          TNonRef *src = static_cast<TNonRef *>(ptr1);
          src->~TNonRef();
          return true;
        }
        case 3: // Get size
        {
          int &ret = *static_cast<int *>(ptr1);
          ret = sizeof(TNonRef);
          return true;
        }
        default:
          return false;
        }
      });
}

template <typename T> void ImSearch::PopSearchable(T &&callback) {
  using TNonRef = typename Internal::remove_reference<T>::type;
  TNonRef moveable{static_cast<decltype(callback)>(callback)};
  Internal::PopSearchable(
      &moveable, +[](int mode, void *ptr1, void *ptr2) {
        switch (mode) {
        case 0: // Invoke
        {
          TNonRef *func = static_cast<TNonRef *>(ptr1);
          (*func)();
          return true;
        }
        case 1: // Move-construct
        {
          TNonRef *src = static_cast<TNonRef *>(ptr1);
          TNonRef *dst = static_cast<TNonRef *>(ptr2);
          new (dst) TNonRef(static_cast<TNonRef &&>(*src));
          return true;
        }
        case 2: // Destructor
        {
          TNonRef *src = static_cast<TNonRef *>(ptr1);
          src->~TNonRef();
          return true;
        }
        case 3: // Get size
        {
          int &ret = *static_cast<int *>(ptr1);
          ret = sizeof(TNonRef);
          return true;
        }
        default:
          return false;
        }
      });
}

#endif // #ifndef IMGUI_DISABLE
