/** copied from
* https://www.fluentcpp.com/2019/08/30/how-to-disable-a-warning-in-cpp/
*/

#if defined(_MSC_VER)
#define DISABLE_WARNING_PUSH __pragma(warning(push))
#define DISABLE_WARNING_POP __pragma(warning(pop))
#define DISABLE_WARNING(warningNumber)                                         \
    __pragma(warning(disable : warningNumber))

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER DISABLE_WARNING(4100)
#define DISABLE_WARNING_UNREFERENCED_FUNCTION DISABLE_WARNING(4505)
#define DISABLE_WARNING_CONSTANT_CONDITIONAL DISABLE_WARNING(4127)
#define DISABLE_WARNING_UNDEF
#define DISABLE_WARNING_SHADOW

// other warnings you want to deactivate...

#elif defined(__GNUC__) || defined(__clang__)
#define DO_PRAGMA(X) _Pragma(#X)
#define DISABLE_WARNING_PUSH DO_PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING_POP DO_PRAGMA(GCC diagnostic pop)
#define DISABLE_WARNING(warningName)                                           \
    DO_PRAGMA(GCC diagnostic ignored #warningName)

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER                          \
    DISABLE_WARNING(-Wunused-parameter)
#define DISABLE_WARNING_UNREFERENCED_FUNCTION                                  \
    DISABLE_WARNING(-Wunused-function)
#define DISABLE_WARNING_CONSTANT_CONDITIONAL
#define DISABLE_WARNING_UNDEF DISABLE_WARNING(-Wundef)
#define DISABLE_WARNING_SHADOW DISABLE_WARNING(-Wshadow)

#else
#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING_POP
#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER
#define DISABLE_WARNING_UNREFERENCED_FUNCTION
#define DISABLE_WARNING_CONSTANT_CONDITIONAL
#define DISABLE_WARNING_UNDEF
#define DISABLE_WARNING_SHADOW

#endif