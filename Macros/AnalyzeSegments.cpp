#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <cmath>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLorentzVector.h>

void AnalyzeSegments(const char* segmentPath) {
    const double pi = 2 * acos(0.0);

    // Open the input ROOT file for the specific segment
    TFile *f = TFile::Open(segmentPath);
    if (!f || f->IsZombie()) {
        std::cerr << "Failed to open file " << segmentPath << std::endl;
        return;
    }

    TTree *T = static_cast<TTree*>(f->Get("T"));
    if (!T) {
        std::cerr << "Tree T not found in file " << segmentPath << std::endl;
        f->Close();
        return;
    }

    // Create output ROOT file based on the input segment name
    std::string outputFileName = std::string("/sphenix/user/patsfan753/tutorials/tutorials/CaloDataAnaRun24pp/output/44686/output_") + std::filesystem::path(segmentPath).filename().replace_extension(".root").string();
    TFile *fout = new TFile(outputFileName.c_str(), "RECREATE");

    // Branches and variables for reading data
    std::vector<float> *c_E = nullptr, *c_Ecore = nullptr, *c_Phi = nullptr, *c_Eta = nullptr, *c_Pt = nullptr;
    std::vector<float> *c_Chi2 = nullptr, *c_Ntow = nullptr, *c_TowMaxE = nullptr, *c_EnotEcore = nullptr;
    std::vector<bool> *triggerbits = nullptr;
    float totalEEmcal = 0.0;

    // Set branch addresses
    T->SetBranchAddress("clusterECore", &c_E);
    T->SetBranchAddress("clusterPhi", &c_Phi);
    T->SetBranchAddress("clusterEta", &c_Eta);
    T->SetBranchAddress("clusterPt", &c_Pt);
    T->SetBranchAddress("clusterChi2", &c_Chi2);
    T->SetBranchAddress("clusterNtow", &c_Ntow);
    T->SetBranchAddress("clusterTowMaxE", &c_TowMaxE);
    T->SetBranchAddress("triggerVector", &triggerbits);
    T->SetBranchAddress("totalCaloEEMCal", &totalEEmcal);

    // Histograms for analysis
    TH1F *invariantMass = new TH1F("invariantMass", "EMCal energy", 100, 0, 1);
    TH1F *h_trig = new TH1F("h_trignum", "trigger that fired", 65, -0.5, 64.5);
    TH1F *h_c_E = new TH1F("h_clusterECore", "Cluster Core Energy Distribution", 100, 0, 110);

    // Variables for processing events
    float maxChi2 = 10.0, minc_E1 = 1.3, minc_E2 = 0.7, maxasym = 0.6;
    int trignum, clus_bigger;
    double maxE, asym;

    // Processing events
    Long64_t nEntries = T->GetEntries();
    for (Long64_t ievent = 0; ievent < nEntries; ievent++) {
        T->GetEntry(ievent);

        if (totalEEmcal <= 0) continue;

        for (size_t i = 0; i < triggerbits->size(); i++) {
            if (triggerbits->at(i)) trignum = i;
        }
        
        h_trig->Fill(trignum);

        for (size_t clus1 = 0; clus1 < c_E->size(); clus1++) {
            if (c_Chi2->at(clus1) > maxChi2) continue;
            if (c_E->at(clus1) < minc_E2) continue;

            TLorentzVector photon1;
            photon1.SetPtEtaPhiE(c_Pt->at(clus1), c_Eta->at(clus1), c_Phi->at(clus1), c_E->at(clus1));

            for (size_t clus2 = 0; clus2 < c_E->size(); clus2++) {
                if (clus2 <= clus1) continue;
                if (c_Chi2->at(clus2) > maxChi2) continue;
                if (c_E->at(clus2) < minc_E2) continue;

                clus_bigger = c_E->at(clus2) > c_E->at(clus1) ? clus2 : clus1;
                maxE = c_E->at(clus_bigger);
                if (maxE < minc_E1) continue;

                asym = fabs(c_E->at(clus1) - c_E->at(clus2)) / (c_E->at(clus1) + c_E->at(clus2));
                if (asym > maxasym) continue;

                TLorentzVector photon2;
                photon2.SetPtEtaPhiE(c_Pt->at(clus2), c_Eta->at(clus2), c_Phi->at(clus2), c_E->at(clus2));
                TLorentzVector meson = photon1 + photon2;

                invariantMass->Fill(meson.M());
            }
        }
    }

    // Write and close output file
    fout->Write();
    fout->Close();
    f->Close();
}
