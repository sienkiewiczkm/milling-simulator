#include "ImGuiExtensions.hpp"

static bool constCharStringVectorGetter(
    void *vec,
    int idx,
    const char **outText
)
{
    auto *collection = static_cast<vector<string>*>(vec);
    if (collection == nullptr) { return false; }
    if (idx < 0 || idx >= collection->size()) { return false; }
    *outText = collection->at(idx).c_str();
    return true;
}

bool ImGui::Combo(
    const char* label,
    int* currIndex,
    std::vector<std::string> &values)
{
    if (values.empty()) { return false; }
    return Combo(label, currIndex, constCharStringVectorGetter,
        static_cast<void*>(&values), values.size());
}

bool ImGui::ListBox(
    const char* label,
    int* currIndex,
    std::vector<std::string> &values
)
{
    if (values.empty()) { return false; }
    return ListBox(label, currIndex, constCharStringVectorGetter,
        static_cast<void*>(&values), values.size());
}
