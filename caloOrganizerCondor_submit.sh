#!/usr/bin/env bash

# List of run numbers
runNumbers=("52104,52107,52111")

# Loop over each run number and create a custom submission file
for runNumber in "${runNumbers[@]}"; do
  # Create a custom submission file for the current run number
  cat > triggerCondor_${runNumber}.sub <<EOL
universe                = vanilla
executable              = triggerCondor.sh
arguments               = ${runNumber} \$(filename) \$(Cluster)
log                     = /sphenix/tg/tg01/bulk/jbennett/TriggerAna/log/job.\$(Cluster).\$(Process).log
output                  = /sphenix/tg/tg01/bulk/jbennett/TriggerAna/stdout/job.\$(Cluster).\$(Process).out
error                   = /sphenix/tg/tg01/bulk/jbennett/TriggerAna/error/job.\$(Cluster).\$(Process).err
request_memory          = 7GB
queue filename from dst_calo_run2pp-000${runNumber}.list
EOL

  # Submit the job
  condor_submit caloOrganizerCondor_${runNumber}.sub
done
