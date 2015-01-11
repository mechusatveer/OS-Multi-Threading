re-entrant code can be called more than once, even though called by different threads,
it still works correctly. So, the re-entrant section of code usually use local variables 
only in such a way that each and every call to the code gets its own unique copy of data.

Non Rentrant code
=================
int g_var = 1;

int f(){
  g_var = g_var + 2;
  return g_var;
}
int g(){
  return f() + 2;
}

Rentrant code
===================

int f(int i) { 
return i + 2; 
} 
int g(int i) { 
return f(i) + 2; 
} 
