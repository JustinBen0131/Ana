#!/usr/bin/env bash

# List of run numbers
runNumbers=("47495" "47501" "47502" "47503" "47506" "47509" "47516" "47517" "47521" "47524" "47540" "47549" "47552" "47557" "47634" "47635" "47637")

# Loop over each run number and create a custom submission file
for runNumber in "${runNumbers[@]}"; do
  # Create a custom submission file for the current run number
  cat > makeTriggerHistograms_${runNumber}.sub <<EOL
universe                = vanilla
executable              = makeTriggerHistograms.sh
arguments               = ${runNumber} \$(filename) \$(Cluster)
log                     = /sphenix/tg/tg01/bulk/jbennett/TriggerAna/log/job.\$(Cluster).\$(Process).log
output                  = /sphenix/tg/tg01/bulk/jbennett/TriggerAna/stdout/job.\$(Cluster).\$(Process).out
error                   = /sphenix/tg/tg01/bulk/jbennett/TriggerAna/error/job.\$(Cluster).\$(Process).err
request_memory          = 7GB
queue filename from ${runNumber}_segmentList.list
EOL

  # Submit the job
  condor_submit makeTriggerHistograms_${runNumber}.sub
done
