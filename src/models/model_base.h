#pragma once

#include "marian.h"
#include <string>

namespace marian {
namespace models {

class ModelBase {
public:
#ifndef CNTK_BACKEND
  virtual void load(Ptr<ExpressionGraph>,
                    const std::string&,
                    bool markReloaded = true)
      = 0;
  virtual void save(Ptr<ExpressionGraph>,
                    const std::string&,
                    bool saveTranslatorConfig = false)
      = 0;
#endif

  virtual Expr build(Ptr<ExpressionGraph> graph,
                     Ptr<data::Batch> batch,
                     bool clearGraph = true)
      = 0;

#ifndef CNTK_BACKEND
  virtual Ptr<data::BatchStats> collectStats(Ptr<ExpressionGraph> graph,
                                             size_t multiplier = 1)
      = 0;
#endif

  virtual void clear(Ptr<ExpressionGraph> graph) = 0;
};
}
}
