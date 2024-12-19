#pragma once

#include "TipType.h"
#include "TipCons.h"
#include <string>

/*!
 * \class TipBoolean
 *
 * \brief A proper type representing a boolean value
 */

class TipBoolean : public TipCons {
public:
  TipBoolean();

  bool operator==(const TipType &other) const override;
  bool operator!=(const TipType &other) const override;

  void accept(TipTypeVisitor *visitor) override;

protected:
  std::ostream &print(std::ostream &out) const override;
};
