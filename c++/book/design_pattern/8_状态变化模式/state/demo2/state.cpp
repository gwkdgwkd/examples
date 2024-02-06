#include <iostream>

#include "context.h"
#include "state.h"

void State::ChangeState(Context* con, State* st) { con->ChangeState(st); }
