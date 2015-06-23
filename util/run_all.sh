#########################################
# run all the results for a specific parameter setting
# the results include the error rate (super_reports), runtime,
# avg_treeIntv_ratio
#########################################

HALFSIZE_PT=0.004
HALFSIZE_TT=0.95
FULLSIZE_PT=0.064
FULLSIZE_TT=0.6
SUPER_HALFSIZE_PRE=new_pt_$HALFSIZE_PT"_tt_"$HALFIZE_TT
SUPER_FULLSIZE_PRE=new_pt_$FULLSIZE_PT"_tt_"$FULLIZE_TT

#cd ..
#python -u super_run_nonocc.py 2 $SUPER_HALFSIZE_PRE halfsize | tee \
#results/log_"$SUPER_HALFSIZE_PRE"_ge2_halfsize.txt && \
#python -u super_run_nonocc.py 1 $SUPER_FULLSIZE_PRE fullsize | tee \
#results/log_"$SUPER_FULLSIZE_PRE"_ge1_fullsize.txt &&\
cd util/ 
for algo in dp_mst dp_st dp_rt 
do
    echo "~~~~~~~~~~"$algo"~~~~~~~~~~" 
    echo "halfsize-"$algo" time" 
    python get_time_from_log.py \
      ../results/pixel_tree_intv_threshold_sensitivity/log_pixel_tree_intv_threshold_err_ge_1halfsize_pixelIntvThreshold"$HALFSIZE_PT"_treeIntvThreshold"$HALFSIZE_TT".txt  $algo
    echo "fullsize-"$algo" time" 
    python get_time_from_log.py \
      ../results/pixel_tree_intv_threshold_sensitivity/log_pixel_tree_intv_threshold_err_ge_2fullsize_pixelIntvThreshold"$FULLSIZE_PT"_treeIntvThreshold"$FULLSIZE_TT".txt $algo
#    echo "python get_avg_treeIntv_ratio.py $algo halfsize 3" 
#    python get_avg_treeIntv_ratio.py $algo halfsize 3 
#    echo "python get_avg_treeIntv_ratio.py $algo fullsize 3" 
#    python get_avg_treeIntv_ratio.py $algo fullsize 3 
done
