/*
 * graph_construction.hpp
 *
 *  Created on: Aug 12, 2011
 *      Author: sergey
 */

#ifndef GRAPH_CONSTRUCTION_HPP_
#define GRAPH_CONSTRUCTION_HPP_

#include "io/multifile_reader.hpp"
#include "debruijn_graph_constructor.hpp"
#include "omni/edges_position_handler.hpp"
#include "new_debruijn.hpp"
#include "omni/paired_info.hpp"
#include "graphio.hpp"
#include "graph_pack.hpp"
#include "utils.hpp"

namespace debruijn_graph {
typedef io::IReader<io::SingleRead> SingleReadStream;
typedef io::IReader<io::PairedRead> PairedReadStream;
typedef io::MultifileReader<io::SingleRead> CompositeSingleReadStream;
typedef io::ConvertingReaderWrapper UnitedStream;

template<size_t k>
void FillPairedIndexWithReadCountMetric(const Graph &g, const IdTrackHandler<Graph>& int_ids,
		const EdgeIndex<k + 1, Graph>& index
		, const KmerMapper<k + 1, Graph>& kmer_mapper
		, PairedInfoIndex<Graph>& paired_info_index , io::IReader<io::PairedRead>& stream) {
	stream.reset();
	INFO("Counting paired info with read count weight");
	NewExtendedSequenceMapper<k + 1, Graph> mapper(g, index, kmer_mapper);
	LatePairedIndexFiller<k + 1, Graph, NewExtendedSequenceMapper<k + 1, Graph>> pif(g, mapper, stream, PairedReadCountWeight);
//	ExtendedSequenceMapper<k + 1, Graph> mapper(g, int_ids, index, kmer_mapper);
//	LatePairedIndexFiller<k + 1, Graph, ExtendedSequenceMapper<k + 1, Graph>, ReadStream> pif(g, mapper, stream, PairedReadCountWeight);
	pif.FillIndex(paired_info_index);
	DEBUG("Paired info with read count weight counted");
}

template<size_t k>
void FillPairedIndexWithProductMetric(const Graph &g
		, const EdgeIndex<k + 1, Graph>& index
		, const KmerMapper<k + 1, Graph>& kmer_mapper
		, PairedInfoIndex<Graph>& paired_info_index , io::IReader<io::PairedRead>& stream) {
	stream.reset();
	INFO("Counting paired info with product weight");
	//	ExtendedSequenceMapper<k + 1, Graph> mapper(g, int_ids, index, kmer_mapper);
	//	LatePairedIndexFiller<k + 1, Graph, ExtendedSequenceMapper<k + 1, Graph>, ReadStream> pif(g, mapper, stream, PairedReadCountWeight);
	NewExtendedSequenceMapper<k + 1, Graph> mapper(g, index, kmer_mapper);
	LatePairedIndexFiller<k + 1, Graph, NewExtendedSequenceMapper<k + 1, Graph>> pif(g, mapper, stream, KmerCountProductWeight);
	pif.FillIndex(paired_info_index);
	DEBUG("Paired info with product weight counted");
}

template<size_t k>
void FillPairedIndex(const Graph &g, const EdgeIndex<k + 1, Graph>& index
		, PairedInfoIndex<Graph>& paired_info_index,
		io::IReader<io::PairedRead>& stream) {
	typedef SimpleSequenceMapper<k + 1, Graph> SequenceMapper;
	stream.reset();
	INFO("Counting paired info");
	SequenceMapper mapper(g, index);
	PairedIndexFiller<k + 1, Graph, SequenceMapper> pif(g, mapper,
			stream);
	pif.FillIndex(paired_info_index);
	DEBUG("Paired info counted");
}

template<size_t k>
void FillEtalonPairedIndex(PairedInfoIndex<Graph>& etalon_paired_index,
		const Graph &g,
		const EdgeIndex<k + 1, Graph>& index,
		const KmerMapper<k + 1, Graph>& kmer_mapper,
		size_t is, size_t rs,
        size_t delta,
		const Sequence& genome){
	INFO("Counting etalon paired info");
	EtalonPairedInfoCounter<k, Graph> etalon_paired_info_counter(g, index, kmer_mapper,
			is, rs, delta);
	etalon_paired_info_counter.FillEtalonPairedInfo(genome,
			etalon_paired_index);

	DEBUG("Etalon paired info counted");
}

template<size_t k>
void FillEtalonPairedIndex(PairedInfoIndex<Graph>& etalon_paired_index,
		const Graph &g,
		const EdgeIndex<k + 1, Graph>& index,
		const KmerMapper<k+1, Graph>& kmer_mapper,
		const Sequence& genome) {
	FillEtalonPairedIndex<k>(etalon_paired_index, g, index, kmer_mapper, *cfg::get().ds.IS, *cfg::get().ds.RL, size_t(*cfg::get().ds.is_var), genome);
	//////////////////DEBUG
	//	SimpleSequenceMapper<k + 1, Graph> simple_mapper(g, index);
	//	Path<EdgeId> path = simple_mapper.MapSequence(genome);
	//	SequenceBuilder sequence_builder;
	//	sequence_builder.append(Seq<k>(g.EdgeNucls(path[0])));
	//	for (auto it = path.begin(); it != path.end(); ++it) {
	//		sequence_builder.append(g.EdgeNucls(*it).Subseq(k));
	//	}
	//	Sequence new_genome = sequence_builder.BuildSequence();
	//	NewEtalonPairedInfoCounter<k, Graph> new_etalon_paired_info_counter(g, index,
	//			insert_size, read_length, insert_size * 0.1);
	//	PairedInfoIndex<Graph> new_paired_info_index(g);
	//	new_etalon_paired_info_counter.FillEtalonPairedInfo(new_genome, new_paired_info_index);
	//	CheckInfoEquality(etalon_paired_index, new_paired_info_index);
	//////////////////DEBUG
//	INFO("Etalon paired info counted");
}

template<size_t k>
void FillCoverage(Graph& g, SingleReadStream& stream,
		EdgeIndex<k + 1, Graph>& index) {
	typedef SimpleSequenceMapper<k + 1, Graph> SequenceMapper;
	stream.reset();
	INFO("Counting coverage");
	SequenceMapper read_threader(g, index);
	g.coverage_index().FillIndex<SequenceMapper>(stream, read_threader);
	DEBUG("Coverage counted");
}

template<size_t k, class Graph>
void ConstructGraph(Graph& g, EdgeIndex<k + 1, Graph>& index,
		io::IReader<io::SingleRead>& stream) {
	typedef SeqMap<k + 1, typename Graph::EdgeId> DeBruijn;
	INFO("Constructing DeBruijn graph");
	DeBruijn& debruijn = index.inner_index();
	INFO("Processing reads (takes a while)");

	size_t counter = 0;
	size_t rl = 0;
	io::SingleRead r;
	while (!stream.eof()) {
		stream >> r;
		Sequence s = r.sequence();
		debruijn.CountSequence(s);
		rl = max(rl, s.size());
		VERBOSE_POWER(++counter, " reads processed");
	}

//	VERIFY_MSG(!cfg::get().ds.RL.is_initialized() || *cfg::get().ds.RL == rl,
//			"In datasets.info, wrong RL is specified: " + ToString(cfg::get().ds.RL) + ", not " + ToString(rl));
	if (!cfg::get().ds.RL.is_initialized()) {
		cfg::get_writable().ds.RL = rl;
		INFO("Figured out: read length = " << rl);
	}
	INFO("DeBruijn graph constructed, " << counter << " reads used");

	INFO("Condensing graph");
	DeBruijnGraphConstructor<k, Graph> g_c(debruijn);
	g_c.ConstructGraph(g, index);
	DEBUG("Graph condensed");
}

template<size_t k, class Graph>
void ConstructGraph(Graph& g, EdgeIndex<k + 1, Graph>& index,
		io::IReader<io::SingleRead>& stream1, io::IReader<io::SingleRead>& stream2) {
	io::MultifileReader<io::SingleRead> composite_reader(stream1, stream2);
	ConstructGraph<k, Graph>(g, index, composite_reader);
}

template<size_t k>
void ConstructGraphWithCoverage(Graph& g, EdgeIndex<k + 1, Graph>& index,
SingleReadStream& stream, SingleReadStream* contigs_stream = 0) {
	vector<SingleReadStream*> streams;
	streams.push_back(&stream);
	if (contigs_stream) {
		INFO("Additional contigs stream added for construction");
		streams.push_back(contigs_stream);
	}
	CompositeSingleReadStream composite_stream(streams);
	ConstructGraph<k>(g, index, composite_stream);
	//It is not a bug!!! Don't use composite_stream here!!!
	FillCoverage<k>(g, stream, index);
}

template<size_t k>
void ConstructGraphWithPairedInfo(graph_pack<ConjugateDeBruijnGraph, k>& gp,
		PairedInfoIndex<Graph>& paired_index, PairedReadStream& stream,
		SingleReadStream* single_stream = 0,
		SingleReadStream* contigs_stream = 0) {
	UnitedStream united_stream(stream);

	typedef io::MultifileReader<io::SingleRead> MultiFileStream;
	vector<SingleReadStream*> streams;
	if(!cfg::get().etalon_graph_mode) {
		streams.push_back(&united_stream);
	}
	if (single_stream) {
		streams.push_back(single_stream);
	}
	MultiFileStream composite_stream(streams);
	ConstructGraphWithCoverage<k>(gp.g, gp.index, composite_stream, contigs_stream);

	if (cfg::get().etalon_info_mode || cfg::get().etalon_graph_mode)
		FillEtalonPairedIndex<k>(paired_index, gp.g, gp.index, gp.kmer_mapper, gp.genome);
	else
		FillPairedIndex<k>(gp.g, gp.index, paired_index, stream);
}

}

#endif /* GRAPH_CONSTRUCTION_HPP_ */
