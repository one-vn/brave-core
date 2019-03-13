/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_PAGE_GRAPH_GRAPH_ITEM_EDGE_EDGE_STORAGE_H_
#define BRAVE_COMPONENTS_BRAVE_PAGE_GRAPH_GRAPH_ITEM_EDGE_EDGE_STORAGE_H_

#include <string>
#include "brave/third_party/blink/brave_page_graph/graph_item/edge.h"
#include "brave/third_party/blink/brave_page_graph/types.h"

using ::std::string;

namespace brave_page_graph {

class Node;
class NodeActor;
class NodeStorage;
class PageGraph;

class EdgeStorage : public Edge {
friend class PageGraph;
 public:
  EdgeStorage() = delete;

 protected:
  EdgeStorage(const PageGraph* graph, const PageGraphId id,
    const NodeActor* out_node, const NodeStorage* in_node, const string& key);
  string ToStringBody() const override;

  const string key_;
};

}  // namespace brave_page_graph

#endif  // BRAVE_COMPONENTS_BRAVE_PAGE_GRAPH_GRAPH_ITEM_EDGE_EDGE_STORAGE_H_
