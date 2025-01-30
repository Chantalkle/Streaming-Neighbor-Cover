import os
import csv
import random
import re
import networkx as nx

def parse_graph_file(filename):
    """
    Parses a graph file and returns the number of nodes, edges, and the edges as a list of tuples.

    Args:
        filename (str): Path to the input file.

    Returns:
        tuple: (n, m, edges) where
            - n is the number of nodes,
            - m is the number of edges,
            - edges is a list of tuples representing the edges.
    """
    with open(filename, 'r') as file:
        # Read the first line containing node and edge information
        first_line = file.readline().strip()
        match = re.match(r'%n=(\d+),m=(\d+)', first_line)
        if not match:
            raise ValueError("Input file format is incorrect in the first line.")

        n = int(match.group(1))
        m = int(match.group(2))

        # Read the subsequent lines containing the edges
        edges = []
        for line in file:
            line = line.strip()
            if line:  # Ignore empty lines
                u, v = map(int, line.split(','))
                edges.append((min(u, v), max(u, v)))  # Store edges in sorted order

    return n, m, edges

def create_graph_from_edges(n, edges):
    """
    Creates a NetworkX graph from a list of edges.

    Args:
        n (int): Number of nodes in the graph.
        edges (list): List of tuples representing the edges.

    Returns:
        nx.Graph: A NetworkX graph object.
    """
    graph = nx.Graph()
    graph.add_nodes_from(range(1, n + 1))  # Add nodes from 1 to n
    graph.add_edges_from(edges)  # Add edges
    return graph

def generate_edge_stream(n, m, edges, operation_factor):
    """
    Generates a random edge stream that constructs the given graph.

    Args:
        n (int): Number of nodes in the graph.
        m (int): Number of edges in the graph.
        edges (list): List of tuples representing the edges of the graph.
        operation_factor (float): Factor to determine the number of operations (e.g., 2*m).

    Returns:
        list: List of tuples representing the edge stream (u, v, op).
    """
    total_operations = int(operation_factor * m)
    edge_stream = []
    current_edges = set()
    last_used_edge = None

    edge_candidates = list(set(edges))  # Make sure edges are unique
    random.shuffle(edge_candidates)  # Randomize the order of edge processing

    while len(edge_stream) < total_operations:
        if random.random() < 0.5 and len(current_edges) < m:  # Add an edge
            edge = random.choice(edge_candidates)
            if edge != last_used_edge and edge not in current_edges:
                edge_stream.append((edge[0], edge[1], 1))
                current_edges.add(edge)
                last_used_edge = edge
        elif current_edges:  # Remove an edge
            edge = random.choice(list(current_edges))
            if edge != last_used_edge:
                edge_stream.append((edge[0], edge[1], -1))
                current_edges.remove(edge)
                last_used_edge = edge

    # Ensure the final graph matches the input graph, in random order
    remaining_edges = [edge for edge in edges if edge not in current_edges]
    random.shuffle(remaining_edges)
    for edge in remaining_edges:
        edge_stream.append((edge[0], edge[1], 1))
        current_edges.add(edge)

    return edge_stream

def save_edge_stream_to_file(n, m, edge_stream, output_file):
    """
    Saves the edge stream to a text file in the specified format.

    Args:
        n (int): Number of nodes in the graph.
        m (int): Number of edges in the graph.
        edge_stream (list): List of tuples representing the edge stream (u, v, op).
        output_file (str): Path to the output file.
    """
    with open(output_file, 'w') as file:
        file.write(f"%n={n},m={m},o={len(edge_stream)}\n")
        for u, v, op in edge_stream:
            file.write(f"{u},{v},{op}\n")

if __name__ == "__main__":
    input_csv = r"C:\Users\Chantal\Desktop\Uni\Masterarbeit\networkx\graph_info_30_11.csv"#"C:\\Users\\Chantal\\network-repository\\networks.csv"  # Input CSV file
    output_csv = r"C:\Users\Chantal\Desktop\Uni\Masterarbeit\networkx\graph_info_dyn_19_12.csv" #"C:\\Users\\Chantal\\network-repository\\networks-dyn_t_bis_ht.csv"  # Output CSV file
    base_path = ""#"C:\\Users\\Chantal\\network-repository"  # Base path for graph files

    operation_factors = [1.1, 1.2, 1.5]  # Example operation factors
    output_rows = []

    # Read the input CSV file
    with open(input_csv, 'r') as infile:
        reader = csv.reader(infile)
        next(reader)  # Skip header row
        for row in reader:
            graph_file, n, m = row[0], int(row[1]), int(row[2])
            graph_path = os.path.join(base_path, graph_file)

            # Check if the number of nodes is less than 1000
            if n < 1000 or n > 100000:
                print(f"Skipping {graph_file} as it has {n} nodes (>= 1000).")
                continue

            if not os.path.exists(graph_path):
                print(f"File {graph_path} not found. Skipping...")
                continue

            print(f"Processing {graph_file}...")
            n, m, edges = parse_graph_file(graph_path)

            for factor in operation_factors:
                edge_stream = generate_edge_stream(n, m, edges, factor)

                output_file = os.path.join(base_path, f"{os.path.splitext(graph_file)[0]}_dyn_{factor:.1f}.txt")
                save_edge_stream_to_file(n, m, edge_stream, output_file)

                output_rows.append([output_file, n, m, len(edge_stream)])

    # Write the output CSV file
    with open(output_csv, 'w', newline='') as outfile:
        writer = csv.writer(outfile)
        writer.writerow(["graphfile", "nodes", "edges", "operations"])
        writer.writerows(output_rows)

    print(f"Processing complete. Output CSV saved to {output_csv}.")
