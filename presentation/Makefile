MERGE_FILES=../merge_results.py
PLOT=../overview.py

RESULTS_DIR=../results

RESULTS_OPENMPI_DEFAULT=$(RESULTS_DIR)/openmpi/5.0.2/default/
RESULTS_OPENMPI_UCX=$(RESULTS_DIR)/openmpi/5.0.2/ucx/
RESULTS_OPENMPI_OB1=$(RESULTS_DIR)/openmpi/5.0.2/ob1/

RESULTS_MPICH=$(RESULTS_DIR)/mpich/4.1.2/

PLOTS_DIR=./

.phony: all
all: fig_mpich_psend_isend_threaded.png fig_mpich_psend_isend.png fig_mpich_psend.png fig_mpich_send.png fig_mpich_tlocal.png fig_mpich_win.png fig_mpich_win_single.png fig_openmpi_isend.png fig_openmpi_ob1_isend.png fig_openmpi_ob1_psend.png fig_openmpi_ob1_send.png fig_openmpi_ob1_tlocal.png fig_openmpi_ob1_win.png fig_openmpi_ob1_win_single.png fig_openmpi_psend_isend.png fig_openmpi_psend_isend_threaded.png fig_openmpi_psend.png fig_openmpi_psend_win.png fig_openmpi_send.png fig_openmpi_tlocal.png fig_openmpi_twait.png fig_openmpi_win.png fig_openmpi_win_single.png

$(RESULTS_OPENMPI_DEFAULT)/results.csv: ;
$(RESULTS_OPENMPI_OB1)/results.csv: ;
$(RESULTS_MPICH)/results.csv: ;

COMMON_FLAGS=-s -y 0,25000000000

fig_openmpi_send.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@           -c bandwidth -m Send         -p linear,random -t MPI_Send $(COMMON_FLAGS)
fig_openmpi_isend.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@          -c bandwidth -m Isend        -p linear,random -t MPI_Isend $(COMMON_FLAGS)
fig_openmpi_psend.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@          -c bandwidth -m Psend        -p linear,random -t MPI_Psend $(COMMON_FLAGS)
fig_openmpi_win.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@            -c bandwidth -m Win          -p linear,random -t 'MPI_Win (one per partition)' $(COMMON_FLAGS)
fig_openmpi_win_single.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@     -c bandwidth -m WinSingle    -p linear,random -t MPI_Win $(COMMON_FLAGS)
                                    
fig_openmpi_tlocal.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@         -c t_local   -m Send,Isend,Psend,Win,WinSingle -p linear -t 'average time per message' $(COMMON_FLAGS)
fig_openmpi_twait.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@          -c t_wait     -m Send,Isend,Psend,Win,WinSingle -p linear -t 'average time in MPI_Wait()' $(COMMON_FLAGS)

fig_openmpi_psend_isend.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@    -c bandwidth -m Psend,Isend  -p linear -t 'Psend vs Isend' $(COMMON_FLAGS)
fig_openmpi_psend_isend_threaded.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@    -c bandwidth -m PsendThreaded,IsendThreaded  -p linear -t 'Psend vs Isend (multithreaded)' -n 1,2,4,8,16,32,64 $(COMMON_FLAGS)

fig_openmpi_psend_win.png: $(RESULTS_OPENMPI_DEFAULT)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_DEFAULT)/results.csv -o $@      -c bandwidth -m Psend,WinSingle  -p linear -t 'Psend vs Win - single' $(COMMON_FLAGS)
      
fig_openmpi_ob1_send.png: $(RESULTS_OPENMPI_OB1)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_OB1)/results.csv -o $@       -c bandwidth -m Send      -p linear,random -t MPI_Send $(COMMON_FLAGS)
fig_openmpi_ob1_isend.png: $(RESULTS_OPENMPI_OB1)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_OB1)/results.csv -o $@      -c bandwidth -m Isend     -p linear,random -t MPI_Isend $(COMMON_FLAGS)
fig_openmpi_ob1_psend.png: $(RESULTS_OPENMPI_OB1)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_OB1)/results.csv -o $@      -c bandwidth -m Psend     -p linear,random -t MPI_Psend $(COMMON_FLAGS)
fig_openmpi_ob1_win.png: $(RESULTS_OPENMPI_OB1)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_OB1)/results.csv -o $@        -c bandwidth -m Win       -p linear,random -t 'MPI_Win (one per partition)' $(COMMON_FLAGS)
fig_openmpi_ob1_win_single.png: $(RESULTS_OPENMPI_OB1)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_OB1)/results.csv -o $@ -c bandwidth -m WinSingle -p linear,random -t MPI_Win $(COMMON_FLAGS)
                                
fig_openmpi_ob1_tlocal.png: $(RESULTS_OPENMPI_OB1)/results.csv
	$(PLOT) -f $(RESULTS_OPENMPI_OB1)/results.csv -o $@     -c t_local   -m Send,Isend,Psend,Win,WinSingle -p linear,random -t 'time per message'
      
fig_mpich_send.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@       -p linear,random -c bandwidth -m Send      -t MPI_Send $(COMMON_FLAGS)
fig_mpich_isend.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@      -p linear,random -c bandwidth -m Isend     -t MPI_Isend $(COMMON_FLAGS)
fig_mpich_psend.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@      -p linear,random -c bandwidth -m Psend     -t MPI_Psend$(COMMON_FLAGS)
fig_mpich_win.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@        -p linear,random -c bandwidth -m Win       -t 'MPI_Win (one per partition)' $(COMMON_FLAGS)
fig_mpich_win_single.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@     -p linear,random -c bandwidth -m WinSingle -t MPI_Win $(COMMON_FLAGS)
      
fig_mpich_tlocal.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@     -c t_local   -m Send,Isend,Psend,Win,WinSingle -p linear,random -t 'time per message' $(COMMON_FLAGS)
      
fig_mpich_psend_isend.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@     -p linear -c bandwidth -m Psend,Isend -t 'MPICH: Isend vs Psend' $(COMMON_FLAGS)
fig_mpich_psend_isend_threaded.png: $(RESULTS_MPICH)/results.csv
	$(PLOT) -f $(RESULTS_MPICH)/results.csv -o $@      -p linear -c bandwidth -m PsendThreaded,IsendThreaded     -t 'Psend vs Isend (multithreaded)' -n 1,2,4,8,16,32,64 $(COMMON_FLAGS)
	
