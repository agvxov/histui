// @BAKE clang -shared -fPIC -Isqlite3 -o damerau_levenshtein.sqlext damerau_levenshtein.c
#include <sqlite3.h>

#include <stdlib.h>
#include <string.h>
#include <limits.h>

// helpers
int minimum(const int a, const int b, const int c);

/* internal function for damerau-levenshtein distance calculation
 */
static int 
damerau_levenshtein_(
  int n,
  const char *const s,
  int m,
  const char *const t
){
  //Step 1
  if ((n == 0) || (m == 0)) {
    return 0;
  }
  
  int *d = (int*)malloc(sizeof(int)*(m+1)*(n+1));
  m++;
  n++;
  
  //Step 2
  int k;
  for(k=0;k<n;k++) d[k]=k;
  for(k=0;k<m;k++) d[k*n]=k;
  
  //Step 3 and 4	
  int i, j;
  for(i=1; i<n; i++) {
    for(j=1; j<m; j++) {
      int cost;
      
      //Step 5
      //cost = s[i-1] != t[j-1];
      if (s[i-1] == t[j-1]) {
        cost = 0;
      }  
      else {
        cost = 1;
      }
        
      //Step 6			 
      d[j*n+i] = minimum(d[(j-1)*n+i]+1, d[j*n+i-1]+1, d[(j-1)*n+i-1]+cost);
      
      //Step 7 – only difference from pure Levenshtein - transposition
      if ( (i > 1) && (j > 1) && (s[i-1] == t[j-2]) && (s[i-2] == t[j-1]) ) {
        d[j*n+i] = minimum(INT_MAX, 
                           d[j*n+i], 
                           d[(j-2)*n+(i-2)] + cost);
      }
    }
  }
  
  const int distance = d[n*m-1];
  free(d);

  return distance;
}

/*
  sqlite3 wrapper to determine damerau-levenshtein distance
  damerau_levenshtein(src,dts) => int
*/
void
damerau_levenshtein(
  sqlite3_context *context,
  [[maybe_unused]] int argc,
  sqlite3_value **argv
){
    
  const char *const s    = (const char *)sqlite3_value_text(argv[0]);
  const char *const t    = (const char *)sqlite3_value_text(argv[1]);
  const int n            = strlen(s); 
  const int m            = strlen(t);

  const int distance = damerau_levenshtein_(n, s, m, t);

  sqlite3_result_int(context, distance);
}

// XXX
void
damerau_levenshtein_substring(
  sqlite3_context *context,
  [[maybe_unused]] int argc,
  sqlite3_value **argv
){
    
  const char *const s    = (const char *)sqlite3_value_text(argv[0]);
  const char *const t    = (const char *)sqlite3_value_text(argv[1]);
  int n = strlen(s); 
  int m = strlen(t);
  n = (n < m ? n : m);
  m = n;

  const int distance = damerau_levenshtein_(n, s, m, t);

  sqlite3_result_int(context, distance);
}

/*
  sqlite wrapper to ensure damerau-levenshtein distance
  damerau_levenshtein(src,dts,max_distance) => bool
*/
void 
is_damerau_levenshtein(
  sqlite3_context *context,
  [[maybe_unused]] int argc,
  sqlite3_value **argv
){
    
  const char *const s    = (const char *)sqlite3_value_text(argv[0]);
  const char *const t    = (const char *)sqlite3_value_text(argv[1]);
  const int n            = strlen(s); 
  const int m            = strlen(t);
  const int max_distance = sqlite3_value_int(argv[2]);
  
  if (abs(n - m) > max_distance) {
    sqlite3_result_int(context, 0);
    return;
  }

  const int distance = damerau_levenshtein_(n, s, m, t);
  
  sqlite3_result_int(context, distance <= max_distance);
}


inline int minimum(const int a, const int b, const int c) {
  int min = a;
  if (b < min) min=b;
  if (c < min) min=c;
  return min;
}
