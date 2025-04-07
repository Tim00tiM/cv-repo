import copy


class Graph:

    def __init__(self, number: int):
        self.vertices = [i for i in range(number)]
        self.edges = {i: [] for i in range(number)}

    def revert_edges(self):
        new_edges = [[] for i in range(len(self.vertices))]
        for i in self.edges:
            for j in self.edges[i]:
                new_edges[j].append(i)
        for i in range(len(new_edges)):
            self.edges[i] = new_edges[i]


def dfs_one_component(current_vertex: int, graph: Graph, used: list[bool], component: list[int]):
    used[current_vertex] = True
    for i in graph.edges[current_vertex]:
        if used[i]:
            continue
        dfs_one_component(i, graph, used, component)
    component.append(current_vertex)


def dfs(current_vertex: int, ids: list[int], graph: Graph,
        result_bypass: list[int], used: list[int]):
    if used[current_vertex]:
        return
    used[current_vertex] = True
    for i in graph.edges[current_vertex]:
        if used[i]:
            continue
        dfs(i, ids, graph, result_bypass, used)
    result_bypass.append(current_vertex)


def dfs_components(graph: Graph, ids: list[int], components: list[list[int]]):
    def dfs_kosaraju(current_vertex: int, graph: Graph, used: list[bool],
                     tin: list[int], tout: list[int], sorted_vertices: list[int],
                     timer: int):
        if used[current_vertex]:
            return
        used[current_vertex] = True
        timer += 1
        tin[current_vertex] = timer
        for i in graph.edges[current_vertex]:
            if used[i]:
                continue
            dfs_kosaraju(i, graph, used, tin, tout, sorted_vertices, timer)
        timer += 1
        tout[current_vertex] = timer
        sorted_vertices.append(current_vertex)

    tin = [0 for i in range(len(graph.vertices))]
    tout = [0 for i in range(len(graph.vertices))]
    timer = 0
    sorted_vertices = []
    used = [False for _ in range(len(graph.vertices))]
    for i in range(len(graph.vertices)):
        dfs_kosaraju(i, graph, used, tin, tout, sorted_vertices, timer)
    sorted_vertices = sorted_vertices[::-1]
    graph2 = copy.copy(graph)
    graph2.revert_edges()
    used = [False for _ in range(len(graph.vertices))]
    id = 0
    for i in range(len(graph2.vertices)):
        new_component = []
        if not used[sorted_vertices[i]]:
            dfs_one_component(sorted_vertices[i], graph2, used, new_component)
            components.append(new_component)
            for j in new_component:
                ids[j] = id
            id += 1
