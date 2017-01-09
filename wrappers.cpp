#include "utils.h"

bool join(KP kpi) {
  Triple foo;
  composeMessage('j', kpi, foo);
}

//------------------------------------------------------------------------------------------------------------------------------------

bool rdfInsert(KP kpi, Triple cont) {
  composeMessage('i', kpi, cont);
}

