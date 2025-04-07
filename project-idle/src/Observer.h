#pragma once

struct Observer {
  virtual void HandleEvent() = 0;
};