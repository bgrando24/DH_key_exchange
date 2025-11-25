#ifndef CONTEXT_HANDLER_HPP
#define CONTEXT_HANDLER_HPP

#include "participant.hpp"

/**
  The DHKE Context Manager class provides a standardised environment for the key exchange participants
 */
class DHKEContextManager
{
private:
  // bob is participant 1, alice is participant 2
  DHKEParticipant *bob;
  DHKEParticipant *alice;

public:
  DHKEContextManager(DHKEParticipant bob, DHKEParticipant alice)
  {
    this->bob = &bob;
    this->alice = &alice;
  }
};

#endif