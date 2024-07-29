MERGE_FILES=../processing/merge_results.py
PLOT=../processing/overview.py

RESULTS_DIR=../results

RESULTS_OPENMPI_DEFAULT=$(RESULTS_DIR)/openmpi/5.0.3/default/
RESULTS_OPENMPI_UCX=$(RESULTS_DIR)/openmpi/5.0.3/ucx/
RESULTS_OPENMPI_OB1=$(RESULTS_DIR)/openmpi/5.0.3/ob1/

RESULTS_MPICH=$(RESULTS_DIR)/mpich/4.1.2/

THREAD_COUNTS='1,2,4,8,16,32,64'

PLOTS_DIR=./

.phony: all
all: fig_openmpi_send.png fig_openmpi_persist.png fig_openmpi_persist_threaded.png fig_openmpi_isend.png fig_openmpi_isend_threaded.png fig_openmpi_psend.png fig_openmpi_psend_threaded.png fig_openmpi_win.png fig_openmpi_win_single.png fig_openmpi_tlocal.png fig_openmpi_twait.png fig_openmpi_psend_isend.png fig_openmpi_psend_isend_threaded.png fig_openmpi_psend_win.png fig_openmpi_ob1_send.png fig_openmpi_ob1_isend.png fig_openmpi_ob1_psend.png fig_openmpi_ob1_win.png fig_openmpi_ob1_win_single.png fig_openmpi_ob1_tlocal.png fig_mpich_send.png fig_mpich_isend.png fig_mpich_isend_threaded.png fig_mpich_psend.png fig_mpich_psend_threaded.png fig_mpich_win.png fig_mpich_win_single.png fig_mpich_tlocal.png fig_mpich_psend_isend.png fig_mpich_psend_isend_threaded.png fig_openmpi_win_single_threaded.png fig_openmpi_psend_parrived.png fig_openmpi_psend_progress.png fig_openmpi_psend_progress_threaded.png fig_openmpi_isend_get_status.png fig_openmpi_isend_get_status_threaded.png fig_mpich_psend_progress.png fig_mpich_psend_parrived.png fig_mpich_psend_progress_threaded.png fig_mpich_isend_get_status.png fig_mpich_isend_get_status_threaded.png fig_openmpi_psend_patterns.png fig_openmpi_ttotal.png

$(RESULTS_OPENMPI_DEFAULT)/results.csv:
	$(MERGE_FILES) $(RESULTS_OPENMPI_DEFAULT)/R0.csv $(RESULTS_OPENMPI_DEFAULT)/R1.csv $(RESULTS_OPENMPI_DEFAULT)/results.csv 
$(RESULTS_OPENMPI_OB1)/results.csv:
	$(MERGE_FILES) $(RESULTS_OPENMPI_OB1)/R0.csv $(RESULTS_OPENMPI_OB1)/R1.csv $(RESULTS_OPENMPI_OB1)/results.csv 
$(RESULTS_MPICH)/results.csv:
	$(MERGE_FILES) $(RESULTS_MPICH)/R0.csv $(RESULTS_MPICH)/R1.csv $(RESULTS_MPICH)/results.csv 

COMMON_FLAGS=-s -y 0,25000000000 -x 512,8388608 -c bandwidth
COMMON_FLAGS_TIME=-s

# plots of bandwidth for each mechanism
fig_openmpi_send.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m Send         -p linear -t MPI_Send $(COMMON_FLAGS)
fig_openmpi_persist.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m SendPersistent -p linear -t 'Persistent MPI_Send' $(COMMON_FLAGS)
fig_openmpi_persist_threaded.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m SendPersistent -p linear -t 'Persistent MPI_Send' -n $(THREAD_COUNTS) $(COMMON_FLAGS)
fig_openmpi_isend.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m Isend        -p linear -t MPI_Isend $(COMMON_FLAGS)
fig_openmpi_isend_threaded.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m Isend        -p linear -t MPI_Isend -n $(THREAD_COUNTS) $(COMMON_FLAGS)
fig_openmpi_psend.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m Psend        -p linear -t MPI_Psend $(COMMON_FLAGS)
fig_openmpi_psend_patterns.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m Psend        -p '*' -t MPI_Psend $(COMMON_FLAGS)
fig_openmpi_psend_threaded.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m Psend        -p linear -t MPI_Psend -n $(THREAD_COUNTS) $(COMMON_FLAGS)
fig_openmpi_win.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m Win          -p linear -t 'MPI_Win (one per partition)' $(COMMON_FLAGS)
fig_openmpi_win_single.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m WinSingle    -p linear -t MPI_Win $(COMMON_FLAGS)
fig_openmpi_win_single_threaded.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m WinSingle    -p linear -t MPI_Win -n $(THREAD_COUNTS) $(COMMON_FLAGS)

# with completion testing
fig_openmpi_psend_parrived.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m PsendParrived,Psend  -p linear -t 'MPI_Psend with/without completion test' $(COMMON_FLAGS)
fig_openmpi_psend_progress.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m PsendProgress,Psend  -p linear -t 'MPI_Psend with/without completion test' $(COMMON_FLAGS)
fig_openmpi_psend_progress_threaded.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m PsendProgress        -p linear -t MPI_Psend -n $(THREAD_COUNTS) $(COMMON_FLAGS)
fig_openmpi_isend_get_status.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m IsendTest        -p linear -t 'MPI_Isend with MPI_Request_get_status call' $(COMMON_FLAGS)
fig_openmpi_isend_get_status_threaded.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m IsendTest        -p linear -t 'MPI_Isend with MPI_Request_get_status call' -n $(THREAD_COUNTS) $(COMMON_FLAGS)
                                    
# plots of times for all mechanisms in one
fig_openmpi_tlocal.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -c t_local -m Send,Isend,Psend,Win,WinSingle -p linear -t 'average time per message' $(COMMON_FLAGS_TIME)
fig_openmpi_ttotal.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -c t_total -m Send,Isend,Psend,Win,WinSingle -p linear -t 'average time per message' $(COMMON_FLAGS_TIME)
fig_openmpi_twait.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -c t_wait  -m Send,Isend,Psend,Win,WinSingle -p linear -t 'average time in MPI_Wait()' $(COMMON_FLAGS_TIME)

# comparison of psend and isend, psend and win
fig_openmpi_psend_isend.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m Psend,Isend  -p linear -t 'Psend vs Isend' $(COMMON_FLAGS)
fig_openmpi_psend_isend_threaded.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m Psend,Isend  -p linear -t 'Psend vs Isend (multithreaded)' -n $(THREAD_COUNTS) $(COMMON_FLAGS)
fig_openmpi_psend_win.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m Psend,WinSingle  -p linear -t 'Psend vs Win - single' $(COMMON_FLAGS)
      
# bandwidth per mechanism using the ob1 pml
fig_openmpi_ob1_send.png: $(RESULTS_OPENMPI_OB1)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_OB1)/results.csv -o $@ -m Send      -p linear -t MPI_Send $(COMMON_FLAGS)
fig_openmpi_ob1_isend.png: $(RESULTS_OPENMPI_OB1)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_OB1)/results.csv -o $@ -m Isend     -p linear -t MPI_Isend $(COMMON_FLAGS)
fig_openmpi_ob1_psend.png: $(RESULTS_OPENMPI_OB1)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_OB1)/results.csv -o $@ -m Psend     -p linear -t MPI_Psend $(COMMON_FLAGS)
fig_openmpi_ob1_win.png: $(RESULTS_OPENMPI_OB1)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_OB1)/results.csv -o $@ -m Win       -p linear -t 'MPI_Win (one per partition)' $(COMMON_FLAGS)
fig_openmpi_ob1_win_single.png: $(RESULTS_OPENMPI_OB1)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_OB1)/results.csv -o $@ -m WinSingle -p linear -t MPI_Win $(COMMON_FLAGS)
                                
# transfer times using the ob1 pml
fig_openmpi_ob1_tlocal.png: $(RESULTS_OPENMPI_OB1)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_OB1)/results.csv -o $@ -c t_local   -m Send,Isend,Psend,Win,WinSingle -p linear -t 'time per message'
      
# bandwidth per mechanism on mpich
fig_mpich_send.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@ -p linear -m Send      -t MPI_Send $(COMMON_FLAGS)
fig_mpich_isend.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@ -p linear -m Isend     -t MPI_Isend $(COMMON_FLAGS)
fig_mpich_isend_threaded.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@ -p linear -m Isend     -t MPI_Isend -n $(THREAD_COUNTS) $(COMMON_FLAGS)
fig_mpich_psend.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@ -p linear -m Psend     -t MPI_Psend $(COMMON_FLAGS)
fig_mpich_psend_threaded.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@ -p linear -m Psend     -t MPI_Psend -n $(THREAD_COUNTS) $(COMMON_FLAGS)
fig_mpich_win.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@ -p linear -m Win       -t 'MPI_Win (one per partition)' $(COMMON_FLAGS)
fig_mpich_win_single.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@ -p linear -m WinSingle -t MPI_Win $(COMMON_FLAGS)

# with completion testing
fig_mpich_psend_parrived.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m PsendParrived,Psend  -p linear -t 'MPI_Psend with/without completion test' $(COMMON_FLAGS)
fig_mpich_psend_progress.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m PsendProgress,Psend  -p linear -t 'MPI_Psend with/without completion test' $(COMMON_FLAGS)
fig_mpich_psend_progress_threaded.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m PsendProgress        -p linear -t MPI_Psend -n $(THREAD_COUNTS) $(COMMON_FLAGS)
fig_mpich_isend_get_status.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m IsendTest        -p linear -t 'MPI_Isend with MPI_Request_get_status call' $(COMMON_FLAGS)
fig_mpich_isend_get_status_threaded.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@ -m IsendTest        -p linear -t 'MPI_Isend with MPI_Request_get_status call' -n $(THREAD_COUNTS) $(COMMON_FLAGS)
 
      
# time for all mechanisms on mpich
fig_mpich_tlocal.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@ -c t_local   -m Send,Isend,Psend,Win,WinSingle -p linear -t 'time per message' $(COMMON_FLAGS_TIME)
      
# comparison of psend and isend, psend and win
fig_mpich_psend_isend.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@ -p linear -m Psend,Isend -t 'MPICH: Isend vs Psend' $(COMMON_FLAGS)
fig_mpich_psend_isend_threaded.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@ -p linear -m Psend,Isend -t 'MPICH: Psend vs Isend (multithreaded)' -n 2,4,8 $(COMMON_FLAGS)
