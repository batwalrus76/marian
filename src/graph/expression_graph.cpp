#include "graph/expression_graph.h"
#include <sstream>

#include "tensors/tensor_operators.h"

namespace marian {

ExpressionGraph::ExpressionGraph(bool inference, bool optimized)
    : inferenceOnly_(inference), optimized_(optimized), backend_(nullptr) {}

void ExpressionGraph::setDevice(DeviceId deviceId) {
  if(!backend_) {
    backend_ = BackendByDevice(deviceId, Config::seed);
    params_ = New<Parameters>();
    params_->init(backend_);
    tensors_ = New<Tensors>(backend_);
  }
}

Expr ExpressionGraph::dropout(float prob, const Shape& shape) {
  return Expression<ConstantNode>(
      shared_from_this(), shape, [prob, this](Tensor t) { Dropout(t, prob); });
}

void ExpressionGraph::checkNan(Tensor t) {
  ABORT_IF(throwNaN_, "Not implemented");
  // ABORT_IF(throwNaN_ && IsNan(t), "Tensor has NaN");
}

void ExpressionGraph::parametersToItems(
    std::vector<io::Item>& ioItems,
    const std::map<std::string, std::string>& nameMap) {
  for(auto p : params()->getMap()) {
    std::string pName = p.first;

    if(!namespace_.empty()) {
      if(pName.substr(0, namespace_.size() + 2) == namespace_ + "::")
        pName = pName.substr(namespace_.size() + 2);
    }

    auto it = nameMap.find(pName);
    if(it != nameMap.end())
      pName = it->second;

    ABORT_IF(p.second->val()->type() != Type::float32,
             "Only float32 supported at the moment");

    Tensor val = p.second->val();

    io::Item item;
    item.name = pName;
    item.shape = val->shape();
    item.type = val->type();

    // Use the actual memory as this will be aligned and padded.
    // When memory mapping this is required. Shape keeps track of
    // tensor size. Saving to *.npz will cut to size.
    auto mem = val->memory();
    item.bytes.resize(mem->size());
    copy(backend_,
         mem->data<char>(),
         mem->data<char>() + mem->size(),
         item.bytes.data());

    ioItems.emplace_back(std::move(item));
  }
}

}  // namespace marian
