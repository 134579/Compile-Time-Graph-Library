#pragma once

#include "list.h"
#include "utility.h"

namespace ctgl {

    // Declarations
    // -------------------------------------------------------------------------

    namespace graph {
        // Node represents a node with the |ID| identifier.
        template <int ID>
        struct Node {
            static constexpr int id = ID;
        };

        // Edge represents a directed edge from the tail Node |T| to the head
        // Node |H| with weight |W|.
        template<typename T, typename H, int W>
        struct Edge {
            using Tail = T;
            using Head = H;
            static constexpr int weight = W;
        };

        // Graph represents a graph consisting of the |N| nodes and |E| edges.
        template <typename N, typename E>
        struct Graph {
            using Nodes = N;
            using Edges = E;
        };

        // Finds all Nodes adjacent to the given Node in the provided Graph.
        template <typename G, typename N>
        constexpr auto getAdjacentNodes(G, N) noexcept;

        // Finds all Nodes connected to the given Node in the provided Graph.
        template <typename G, typename N>
        constexpr auto getConnectedNodes(G, N) noexcept;

        // Finds all outgoing Edges from the given Node in the provided Graph.
        template <typename G, typename N>
        constexpr auto getOutgoingEdges(G, N) noexcept;

        // Reports whether the provided Graph is a strongly-connected component.
        template <typename G>
        constexpr bool isConnected(G) noexcept;

        // Reports whether Node |T| is reachable from Node |S| in the provided Graph.
        template <typename G, typename S, typename T>
        constexpr bool isConnected(G, S, T) noexcept;
    }

    // Definitions
    // -------------------------------------------------------------------------

    namespace graph {
        template <typename G, typename N>
        constexpr auto getAdjacentNodes(G, N) noexcept {
            return list::unique(getAdjacentNodes(N{}, typename G::Edges{}));
        }

        template <typename N, typename H, int W, typename... Es>
        constexpr auto getAdjacentNodes(N, List<Edge<N, H, W>, Es...>) noexcept {
            // The first Edge in the List originates from the source Node.
            return H{} + getAdjacentNodes(N{}, List<Es...>{});
        }

        template <typename N, typename E, typename... Es>
        constexpr auto getAdjacentNodes(N, List<E, Es...>) noexcept {
            // The first Edge in the List does NOT originate from the source Node.
            return getAdjacentNodes(N{}, List<Es...>{});
        }

        template <typename N>
        constexpr auto getAdjacentNodes(N, List<>) noexcept {
            // All the Edges have been traversed.
            return List<>{};
        }

        template <typename G, typename N>
        constexpr auto getConnectedNodes(G, N) noexcept {
            constexpr bool feasible = list::contains(N{}, typename G::Nodes{});
            if constexpr (feasible) {
                constexpr auto next = getAdjacentNodes(G{}, N{});
                constexpr auto span = getConnectedNodes(G{}, N{}, next, List<N>{});
                return list::unique(span);
            } else {
                return List<>{};
            }
        }

        template <typename G, typename N, typename T, typename... Ts, typename... Ps>
        constexpr auto getConnectedNodes(G, N, List<T, Ts...>, List<Ps...>) noexcept {
            constexpr auto skip = getConnectedNodes(G{}, N{}, List<Ts...>{}, List<Ps...>{});
            constexpr auto cycle = list::contains(T{}, List<Ps...>{});
            if constexpr (cycle) {
                return skip;
            } else {
                constexpr auto next = getAdjacentNodes(G{}, T{});
                constexpr auto take = getConnectedNodes(G{}, T{}, next, List<T, Ps...>{});
                return skip + take;
            }
        }

        template <typename G, typename N, typename... Ps>
        constexpr auto getConnectedNodes(G, N, List<>, List<Ps...>) noexcept {
            return List<Ps...>{};
        }

        template <typename G, typename N>
        constexpr auto getOutgoingEdges(G, N) noexcept {
            return list::unique(getOutgoingEdges(N{}, typename G::Edges{}));
        }

        template <typename N, typename E, typename... Es>
        constexpr auto getOutgoingEdges(N, List<E, Es...>) noexcept {
            constexpr bool match = std::is_same_v<N, typename E::Tail>;
            constexpr auto after = getOutgoingEdges(N{}, List<Es...>{});
            if constexpr (match) {
                return E{} + after;
            } else {
                return after;
            }
        }

        template <typename N>
        constexpr auto getOutgoingEdges(N, List<>) noexcept {
            return List<>{};
        }

        template <typename G>
        constexpr bool isConnected(G) noexcept {
            // A Graph is a strongly-connected component if there exists a cycle
            // which includes all Nodes in the Graph.
            constexpr auto nodes = typename G::Nodes{};
            return isConnected(G{}, nodes + nodes);
        }

        template <typename G, typename T1, typename T2, typename... Ts>
        constexpr bool isConnected(G, List<T1, T2, Ts...>) noexcept {
            return isConnected(G{}, T1{}, T2{}) && isConnected(G{}, List<T2, Ts...>{});
        }

        template <typename G, typename... Ts>
        constexpr bool isConnected(G, List<Ts...>) noexcept {
            return true;
        }

        template <typename G, typename S, typename T>
        constexpr bool isConnected(G, S, T) noexcept {
            constexpr auto nodes = typename G::Nodes{};
            constexpr bool hasS = list::contains(S{}, nodes);
            constexpr bool hasT = list::contains(T{}, nodes);
            constexpr bool feasible = hasS && hasT;
            if constexpr (!feasible) {
                return false;
            } else {
                return isConnected(G{}, T{}, List<S>{}, List<>{});
            }
        }

        template <typename G, typename T, typename N, typename... Ns, typename... Ps>
        constexpr bool isConnected(G, T, List<N, Ns...>, List<Ps...>) noexcept {
            constexpr bool cycle = list::contains(N{}, List<Ps...>{});
            if constexpr (cycle) {
                return false;
            } else {
                constexpr auto skip = isConnected(G{}, T{}, List<Ns...>{}, List<Ps...>{});
                constexpr auto next = getAdjacentNodes(G{}, N{});
                constexpr auto take = isConnected(G{}, T{}, next, List<N, Ps...>{});
                return skip || take;
            }
        }

        template <typename G, typename T, typename... Ps>
        constexpr bool isConnected(G, T, List<>, List<Ps...>) noexcept {
            // The neighbourhood is empty.
            return false;
        }

        template <typename G, typename T, typename... Ns, typename... Ps>
        constexpr bool isConnected(G, T, List<T, Ns...>, List<Ps...>) noexcept {
            // The next Node in the neighbourhood matches the target Node.
            return true;
        }
    }

    // Convenient Type Definitions
    // -------------------------------------------------------------------------
    template <int ID>
    using Node = ctgl::graph::Node<ID>;

    template<typename T, typename H, int W>
    using Edge = ctgl::graph::Edge<T, H, W>;

    template <typename N, typename E>
    using Graph = ctgl::graph::Graph<N, E>;
}