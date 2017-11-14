// #define ENSURE_TRUE_OTHERWISE_RETURN(condition) if (!(condition)) return;
#define ENSURE_TRUE_OTHERWISE_RETURN(condition, return_value) if (!(condition)) return return_value;
// #define ENSURE_TRUE_OTHERWISE_RETURN_VOID(condition) if (condition == nullptr) return void;
// #define CHECK_EXIT(a)  if ((a) == nullptr) { return void(); }
#define ENSURE_TRUE_OTHERWISE_CONTINUE(condition) if (!(condition)) continue;