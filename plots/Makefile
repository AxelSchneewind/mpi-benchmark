MERGE_FILES=../plotting/merge_results.py
PLOT=../plotting/overview.py

RESULTS_DIR=../results

RESULTS_OPENMPI_PERSIST_DEFAULT=$(RESULTS_DIR)/openmpi/persist/default/
RESULTS_OPENMPI_PERSIST_REGULAR=$(RESULTS_DIR)/openmpi/persist/regular/
RESULTS_OPENMPI_DIRECT_DEFAULT=$(RESULTS_DIR)/openmpi/direct/default/
RESULTS_OPENMPI_DIRECT_REGULAR=$(RESULTS_DIR)/openmpi/direct/regular/
RESULTS_OPENMPI_DIRECT_RB_TREE=$(RESULTS_DIR)/openmpi/direct/rb_tree/
RESULTS_OPENMPI=$(RESULTS_OPENMPI_PERSIST_REGULAR)

THREAD_COUNTS='1,2,4,8,16,32,64'

PLOTS_DIR=./

$(RESULTS_OPENMPI)/results.csv: ;

COMMON_FLAGS=-s -y 0,25000000000 -x 512,8388608 -c bandwidth
COMMON_FLAGS_TIME=-s

# plots of bandwidth for each mechanism
fig_openmpi_psend.png: $(RESULTS_OPENMPI_PERSIST_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_PERSIST_DEFAULT)/results.csv -o $@ -m Psend        -p linear -t MPI_Psend $(COMMON_FLAGS)
fig_openmpi_psend_threaded.png: $(RESULTS_OPENMPI_PERSIST_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_PERSIST_DEFAULT)/results.csv -o $@ -m Psend        -p linear -t MPI_Psend -n $(THREAD_COUNTS) $(COMMON_FLAGS)

# with completion testing
fig_openmpi_psend_parrived.png: $(RESULTS_OPENMPI_PERSIST_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_PERSIST_DEFAULT)/results.csv -o $@ -m PsendParrived,Psend  -p linear -t 'MPI_Psend with/without completion test' $(COMMON_FLAGS)
fig_openmpi_psend_parrived_threaded.png: $(RESULTS_OPENMPI_PERSIST_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_PERSIST_DEFAULT)/results.csv -o $@ -m PsendParrived        -p linear -t MPI_Psend -n $(THREAD_COUNTS) $(COMMON_FLAGS)
                                    
# comparison of psend and isend, psend
fig_openmpi_psend_isend.png: $(RESULTS_OPENMPI_PERSIST_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_PERSIST_DEFAULT)/results.csv -o $@ -m Psend,Isend  -p linear -t 'Psend vs Isend' $(COMMON_FLAGS)
fig_openmpi_psend_isend_threaded.png: $(RESULTS_OPENMPI_PERSIST_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_PERSIST_DEFAULT)/results.csv -o $@ -m Psend,Isend  -p linear -t 'Psend vs Isend (multithreaded)' -n $(THREAD_COUNTS) $(COMMON_FLAGS)
      