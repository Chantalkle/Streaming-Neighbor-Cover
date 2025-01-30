# -*- coding: utf-8 -*-
"""
Created on Fri Jul 19 15:18:57 2024

@author: Chantal
"""

import networkx 
import random
import os
import csv

# Funktion, um die höchste vorhandene Dateinummer im Verzeichnis zu finden
def get_highest_file_number(directory, prefix, suffix):
    if not os.path.exists(directory):
        return 0
    files = [f for f in os.listdir(directory) if f.startswith(prefix) and f.endswith(suffix)]
    numbers = [int(f[len(prefix):-len(suffix)]) for f in files if f[len(prefix):-len(suffix)].isdigit()]
    return max(numbers, default=0)
for i in (5000,10000,50000,100000,250000,500000,750000,900000,999000):
    num_graphs = 10
    n = 1000
    #d = i*10
    m = i
    
    output_dir = 'C:\\Users\\Chantal\\Desktop\\Uni\\Masterarbeit\\networkx\\gnm_random_neu\\n'+str(n)+'_m'+str(m)
    file_prefix = 'graph_'
    file_suffix = '.txt'
    node_weights_file_prefix = 'node_weights_'
    csv_dir = 'C:\\Users\\Chantal\\Desktop\\Uni\\Masterarbeit\\networkx'
    os.makedirs(output_dir, exist_ok=True)
    start_num = get_highest_file_number(output_dir, file_prefix, file_suffix) + 1
    
    for j in range(num_graphs):
        #G = networkx.random_regular_graph(d,n)
        G = networkx.gnm_random_graph(n, m)
        #for node in G.nodes():
         #  G.nodes[node]['weight'] = random.randint(1, 1000)  # zufälliges Gewicht zwischen 1 und 100
       
        
        file_num = start_num +j
        graph_filename = os.path.join(output_dir, f"{file_prefix}{file_num}{file_suffix}")
        for k in range(10):
            filename = os.path.join(output_dir, f"{file_prefix}{file_num}_{k}{file_suffix}")
            edges = list(G.edges())
            random.shuffle(edges)
               
            with open(filename, 'w') as file:
                file.write(f"%n={n},m={m}\n")
                for edge in edges:
                    file.write(f"{edge[0]},{edge[1]}\n")
        
        
      #  node_weights_filename = os.path.join(output_dir, f"{node_weights_file_prefix}{file_num}{file_suffix}")
        
       # with open(node_weights_filename, 'w') as node_weights_file:
        #    for node, data in G.nodes(data=True):
        #        node_weights_file.write(f"{node},{data['weight']}\n")
             
         # Save graph file info to CSV
        csv_filename = os.path.join(csv_dir, 'graph_info_30_11.csv')
        file_exists = os.path.isfile(csv_filename)
        with open(csv_filename, 'a', newline='') as csvfile:  # 'a' mode for appending
            writer = csv.writer(csvfile)
            if not file_exists:
                writer.writerow(['graph_file', 'node_weights_file', 'nodes', 'edges'])
            for i in range(10):
                graph_filename = os.path.join(output_dir, f"{file_prefix}{file_num}_{i}{file_suffix}")
                writer.writerow([graph_filename, n, m])