

#include <iostream>

#include "state.h"

Context::~Context() { delete state_; }
void Context::Handle() { state_->Handle(this); }
void Context::OperationForStateA() { std::cout << "Do operation in State A "; }
void Context::OperationForStateB() { std::cout << "Do operation in State B "; }
void Context::ChangeState(State* state) { this->state_ = state; }
