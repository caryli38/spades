#define BOOST_TEST_MODULE debruijn_test

#include "graphio.hpp"
#include <iostream>
#include "logging.hpp"
#include "test_utils.hpp"
#include "assembly_compare.hpp"


//headers with tests
#include "debruijn_graph_test.hpp"
#include "simplification_test.hpp"
//#include "pair_info_test.hpp"

DECL_PROJECT_LOGGER("dt")

namespace debruijn_graph {

BOOST_AUTO_TEST_CASE( CompareAssemblies ) {
	AssemblyComparer<graph_pack<NonconjugateDeBruijnGraph, 101>> comparer;
//    	io::EasyReader stream1("/home/snurk/assembly_compare/geba_0001_vsc.fasta.gz");
//    	io::EasyReader stream2("/home/snurk/assembly_compare/geba_0001_spades.fasta.gz");
	//todo split N's
//	io::EasyReader stream1("/home/sergey/assembly_compare/geba_0002_allpaths.fasta.gz");
//	io::EasyReader stream2("/home/sergey/assembly_compare/geba_0002_spades.fasta.gz");

	io::EasyReader stream1("/home/sergey/assembly_compare/PGINGIVALIS_LANE2_BH_split.fasta.gz");
	io::EasyReader stream2("/home/sergey/assembly_compare/PGINGIVALIS_LANE2_EVSC.fasta.gz");
	comparer.CompareAssemblies(stream1, stream2, "spades_", "evsc_");
}

//BOOST_AUTO_TEST_CASE( GenerateGraphFragment ) {
//	std::string input_path = "./data/debruijn/HMP_LANE_3_0/K55/latest/saves/simplified_graph";
//	std::string output_path = "./src/test/debruijn/graph_fragments/topology_ec/iter_unique_path";
//	size_t split_threshold = 230;
//	int int_edge_id = 5573881;
//	graph_pack<ConjugateDeBruijnGraph, 55> gp;
//	ScanGraphPack(input_path, gp);
//	//prints only basic graph structure
//	PrintGraphComponentContainingEdge(output_path, gp.g,
//			split_threshold, gp.int_ids, int_edge_id);
//
//	//long way to write to dot file
//	Graph g(55);
//	IdTrackHandler<Graph> int_ids(g);
//	ScanBasicGraph(output_path, g, int_ids);
//	total_labeler_graph_struct graph_struct(g, &int_ids, (const EdgesPositionHandler<Graph>*)0);
//	total_labeler tot_lab(&graph_struct);
//	WriteToDotFile(g,
//			tot_lab, output_path + ".dot",
//			"mygraph", Path<EdgeId>(), Path<EdgeId>());
//}

}

