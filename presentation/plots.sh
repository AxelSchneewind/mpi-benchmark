#!bash

MERGE_FILES="../merge_results.py"
PLOT="../overview.py"

RESULTS_DIR="../results"

RESULTS_OPENMPI_DEFAULT="$RESULTS_DIR/openmpi/5.0.2/default/"
RESULTS_OPENMPI_UCX="$RESULTS_DIR/openmpi/5.0.2/ucx/"
RESULTS_OPENMPI_OB1="$RESULTS_DIR/openmpi/5.0.2/ob1/"

RESULTS_MPICH="$RESULTS_DIR/mpich/4.1.2/"

PLOTS_DIR="."

$PLOT -f $RESULTS_OPENMPI_DEFAULT/results.csv -o 'fig_openmpi_send.png'       -s -c bandwidth -m Send      -p linear,random -t MPI_Send
$PLOT -f $RESULTS_OPENMPI_DEFAULT/results.csv -o 'fig_openmpi_isend.png'      -s -c bandwidth -m Isend     -p linear,random -t MPI_Isend
$PLOT -f $RESULTS_OPENMPI_DEFAULT/results.csv -o 'fig_openmpi_psend.png'      -s -c bandwidth -m Psend     -p linear,random -t MPI_Psend
$PLOT -f $RESULTS_OPENMPI_DEFAULT/results.csv -o 'fig_openmpi_win.png'        -s -c bandwidth -m Win       -p linear,random -t 'MPI_Win (one per partition)'
$PLOT -f $RESULTS_OPENMPI_DEFAULT/results.csv -o 'fig_openmpi_win_single.png' -s -c bandwidth -m WinSingle -p linear,random -t MPI_Win

$PLOT -f $RESULTS_OPENMPI_DEFAULT/results.csv -o 'fig_openmpi_tlocal.png'     -s -c t_local   -m Send,Isend,Psend,Win,WinSingle -p linear,random -t 'time per message'

$PLOT -f $RESULTS_OPENMPI_OB1/results.csv -o 'fig_openmpi_ob1_send.png'       -s -c bandwidth -m Send      -p linear,random -t MPI_Send
$PLOT -f $RESULTS_OPENMPI_OB1/results.csv -o 'fig_openmpi_ob1_isend.png'      -s -c bandwidth -m Isend     -p linear,random -t MPI_Isend
$PLOT -f $RESULTS_OPENMPI_OB1/results.csv -o 'fig_openmpi_ob1_psend.png'      -s -c bandwidth -m Psend     -p linear,random -t MPI_Psend
$PLOT -f $RESULTS_OPENMPI_OB1/results.csv -o 'fig_openmpi_ob1_win.png'        -s -c bandwidth -m Win       -p linear,random -t 'MPI_Win (one per partition)'
$PLOT -f $RESULTS_OPENMPI_OB1/results.csv -o 'fig_openmpi_ob1_win_single.png' -s -c bandwidth -m WinSingle -p linear,random -t MPI_Win

$PLOT -f $RESULTS_OPENMPI_OB1/results.csv -o 'fig_openmpi_ob1_tlocal.png'     -s -c t_local   -m Send,Isend,Psend,Win,WinSingle -p linear,random -t 'time per message'

$PLOT -f $RESULTS_MPICH/results.csv -o 'fig_mpich_send.png'       -p linear,random -s -c bandwidth -m Send      -t MPI_Send
$PLOT -f $RESULTS_MPICH/results.csv -o 'fig_mpich_isend.png'      -p linear,random -s -c bandwidth -m Isend     -t MPI_Isend
$PLOT -f $RESULTS_MPICH/results.csv -o 'fig_mpich_psend.png'      -p linear,random -s -c bandwidth -m Psend     -t MPI_Psend
$PLOT -f $RESULTS_MPICH/results.csv -o 'fig_mpich_win.png'        -p linear,random -s -c bandwidth -m Win       -t 'MPI_Win (one per partition)'
$PLOT -f $RESULTS_MPICH/results.csv -o 'fig_mpich_win_single.png' -p linear,random -s -c bandwidth -m WinSingle -t MPI_Win

$PLOT -f $RESULTS_MPICH/results.csv -o 'fig_mpich_tlocal.png'     -s -c t_local   -m Send,Isend,Psend,Win,WinSingle -p linear,random -t 'time per message'

$PLOT -f $RESULTS_MPICH/results.csv -o 'fig_mpich_isend_vs_psend.png' -p linear,random -s -c bandwidth -m Isend,Psend -t 'MPICH: Isend vs Psend'