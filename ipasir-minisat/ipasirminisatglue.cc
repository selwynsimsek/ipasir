/* Copyright (C) 2011 - 2014, Armin Biere, Johannes Kepler University, Linz */
/* Copyright (C) 2014, Mathias Preiner, Johannes Kepler University, Linz */

/* This file contains code original developped for the Boolector project,
 * but is also available under the standard IPASIR license.
 */
#define __STDC_LIMIT_MACROS
#define __STDC_FORMAT_MACROS
#include "Solver.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>

using namespace std;
using namespace Minisat;

extern "C" {
static const char * sig = "minisat" VERSION;
};

class IPAsirMiniSAT : public Solver {
  vec<Lit> assumptions, clause;
  int szfmap; unsigned char * fmap; bool nomodel;
  unsigned long long calls;
  void reset () { if (fmap) delete [] fmap, fmap = 0, szfmap = 0; }
  Lit import (int lit) { 
    while (abs (lit) > nVars ()) (void) newVar ();
    return mkLit (Var (abs (lit) - 1), (lit < 0));
  }
  void ana () {
    fmap = new unsigned char [szfmap = nVars ()];
    memset (fmap, 0, szfmap);
    for (int i = 0; i < conflict.size (); i++) {
      int tmp = var (conflict[i]);
      assert (0 <= tmp && tmp < szfmap);
      fmap[tmp] = 1;
    }
  }
  double ps (double s, double t) { return t ? s/t : 0; }
public:
  IPAsirMiniSAT () : szfmap (0), fmap (0), nomodel (false), calls (0) {
    // MiniSAT by default produces non standard conforming messages.
    // So either we have to set this to '0' or patch the sources.
    verbosity = 0;
  }
  ~IPAsirMiniSAT () { reset (); }
  void add (int lit) {
    reset ();
    nomodel = true;
    if (lit) clause.push (import (lit));
    else addClause (clause), clause.clear ();
  }
  void assume (int lit) {
    reset ();
    nomodel = true;
    assumptions.push (import (lit));
  }
  int solve () {
    calls++;
    reset ();
    lbool res = solveLimited (assumptions);
    assumptions.clear ();
    nomodel = (res != l_True);
    return (res == l_Undef) ? 0 : (res == l_True ? 10 : 20);
  }
  int val (int lit) {
    if (nomodel) return 0;
    lbool res = modelValue (import (lit));
    return (res == l_True) ? lit : -lit;
  }
  int failed (int lit) {
    if (!fmap) ana ();
    int tmp = var (import (lit));
    assert (0 <= tmp && tmp < nVars ());
    return fmap[tmp] != 0;
  }
};

extern "C" {
#include "ipasir.h"
static IPAsirMiniSAT * import (void * s) { return (IPAsirMiniSAT*) s; }
const char * ipasir_signature () { return sig; }
void * ipasir_init () { return new IPAsirMiniSAT (); }
void ipasir_release (void * s) { delete import (s); }
int ipasir_solve (void * s) { return import (s)->solve (); }
void ipasir_add (void * s, int l) { import (s)->add (l); }
void ipasir_assume (void * s, int l) { import (s)->assume (l); }
int ipasir_val (void * s, int l) { return import (s)->val (l); }
int ipasir_failed (void * s, int l) { return import (s)->failed (l); }
void ipasir_set_terminate (void * s, void * state, int (*callback)(void * state)) { import(s)->setTermCallback(state, callback); }
void ipasir_set_learn (void * s, void * state, int max_length, void (*learn)(void * state, int * clause)) { import(s)->setLearnCallback(state, max_length, learn); }
};
