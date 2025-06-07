// clang formatting preferences

Language: Cpp
BasedOnStyle: LLVM
IndentWidth: 5
TabWidth: 5
UseTab: Never
BreakBeforeBraces: Allman
AllowShortIfStatementsOnASingleLine: false
AllowShortBlocksOnASingleLine: false
AlwaysBreakTemplateDeclarations: Yes
ReflowComments:  true
PointerAlignment: Right
IdentifierNaming:
     FunctionCase: lower_case
     VariableCase: lower_case
     ClassCase: CamelCase
     StructCase: CamelCase
     EnumCase: CamelCase
     EnumConstantCase: UPPER_CASE
     MemberCase: lower_case
     PrivateMemberPrefix: "_"
     ConstantCase: UPPER_CASE
     GlobalConstantCase: UPPER_CASE
IncludeCategories:
     - Regex: '^"(juce|faust)/'
       Priority: 1
     - Regex: '.*'
       Priority: 2