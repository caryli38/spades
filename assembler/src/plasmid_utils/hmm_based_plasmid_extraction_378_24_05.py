import os, errno
import sys
import ntpath
import argparse
import subprocess
from joblib import Parallel, delayed
from glob import glob
from Bio import SeqIO
from parse_blast_xml import parser
from classifier import naive_bayes
from classifier import scikit_multNB

base = os.path.basename(sys.argv[1])
print(base)
print (os.path.splitext(base))
name_file = os.path.splitext(base)[0]

outdir = sys.argv[2]

try:
    os.makedirs(outdir)
except OSError as e:
    if e.errno != errno.EEXIST:
        raise

name=os.path.join(outdir, name_file)

#hmm= "/Nancy/mrayko/db/pfam/Pfam-A.hmm"
hmm= "/Nancy/mrayko/db/plasmid_specific_pfam/378_10fold_plasmid_HMMs.hmm"
list378="/Nancy/mrayko/PlasmidVerify/plasmid_specific_HMMs/378_hmms.txt" 
blastdb=" /Bmo/ncbi_nt_database/nt"


#hmmscan=" /Nancy/mrayko/Libs/hmmer-3.1b2-linux-intel-x86_64/binaries/hmmscan"
hmmsearch = "/Nancy/mrayko/Libs/hmmer-3.1b2-linux-intel-x86_64/binaries/hmmsearch"
prodigal="/Nancy/mrayko/Libs/Prodigal/prodigal"
cbar="/Nancy/mrayko/Libs/cBar.1.2/cBar.pl"


# run hmm
#os.system (prodigal + " -p meta -i " + sys.argv[1] + " -a "+name+"_proteins.fa -o "+name+"_genes.fa 2>"+name+"_prodigal.log" )
os.system (hmmsearch + " --noali  -o "+name+"_out_pfam --cut_nc --tblout "+name+"_tblout --cpu 10 "+ hmm + " "+name+"_proteins.fa")
os.system ("tail -n +4 " + name +"_tblout | head -n -10 | awk '{print $1}'| sed 's/_[^_]*$//g'| sort | uniq > " + name +"_plasmid_contigs_names.txt")
