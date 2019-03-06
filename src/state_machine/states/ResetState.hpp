#ifndef BGEIGIE_POINTCAST_RESETSTATE_HPP
#define BGEIGIE_POINTCAST_RESETSTATE_HPP

#include "state.h"

class ResetState : public State {
 public:
  ResetState(Controller& context);
  virtual ~ResetState() = default;

  void entry_action() override;
  void do_activity() override;
  void exit_action() override;
  void handle_event(Event_enum event_id) override;
 private:

};

#endif //BGEIGIE_POINTCAST_RESETSTATE_HPP