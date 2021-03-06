// ------------------------------------------------------------
// -----                  R3BSofMwpcOnlineSpectra         -----
// -----    Created 29/09/19  by J.L. Rodriguez-Sanchez   -----
// -----           Fill SOFIA online histograms           -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with SOFIA online data
 */

#include "R3BSofMwpcOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BSofMwpcCalData.h"
#include "R3BSofMwpcHitData.h"
#include "THttpServer.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TVector3.h"

#include "TClonesArray.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TMath.h"
#include "TRandom.h"
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;

R3BSofMwpcOnlineSpectra::R3BSofMwpcOnlineSpectra()
    : FairTask("SofMwpcOnlineSpectra", 1)
    , fCalItemsMwpc(NULL)
    , fHitItemsMwpc(NULL)
    , fNameDet("MWPC")
    , fNEvents(0)
{
}

R3BSofMwpcOnlineSpectra::R3BSofMwpcOnlineSpectra(const TString& name, Int_t iVerbose, const TString& namedet)
    : FairTask(name, iVerbose)
    , fCalItemsMwpc(NULL)
    , fHitItemsMwpc(NULL)
    , fNameDet(namedet)
    , fNEvents(0)
{
}

R3BSofMwpcOnlineSpectra::~R3BSofMwpcOnlineSpectra()
{
    LOG(INFO) << "R3BSof" + fNameDet + "OnlineSpectra::Delete instance";
    if (fCalItemsMwpc)
        delete fCalItemsMwpc;
    if (fHitItemsMwpc)
        delete fHitItemsMwpc;
}

InitStatus R3BSofMwpcOnlineSpectra::Init()
{

    LOG(INFO) << "R3BSof" + fNameDet + "OnlineSpectra::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BSof" + fNameDet + "OnlineSpectra::Init FairRootManager not found";
    // header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // get access to mapped data of mwpcs
    fCalItemsMwpc = (TClonesArray*)mgr->GetObject(fNameDet + "CalData");
    if (!fCalItemsMwpc)
    {
        return kFATAL;
    }

    // get access to hit data of mwpcs
    fHitItemsMwpc = (TClonesArray*)mgr->GetObject(fNameDet + "HitData");
    if (!fHitItemsMwpc)
        LOG(WARNING) << "R3BSofMwpcOnlineSpectra: " + fNameDet + "HitData not found";

    // Create histograms for detectors
    TString Name1;
    TString Name2;

    cMWPCCal = new TCanvas(fNameDet + "_cal", fNameDet + " cal info", 10, 10, 800, 700);
    cMWPCCal->Divide(2, 1);

    // MWPC: Cal data
    Name1 = "fh1_" + fNameDet + "_posx_cal";
    Name2 = fNameDet + ": Position X";
    if (fNameDet != "Mwpc3")
        fh1_mwpc_cal[0] = new TH1F(Name1, Name2, 64, 0.5, 64.5);
    else
        fh1_mwpc_cal[0] = new TH1F(Name1, Name2, 288, 0.5, 288.5);
    fh1_mwpc_cal[0]->GetXaxis()->SetTitle("Position X [pads]");
    fh1_mwpc_cal[0]->GetYaxis()->SetTitle("Counts");
    fh1_mwpc_cal[0]->GetYaxis()->SetTitleOffset(1.15);
    fh1_mwpc_cal[0]->GetXaxis()->CenterTitle(true);
    fh1_mwpc_cal[0]->GetYaxis()->CenterTitle(true);
    fh1_mwpc_cal[0]->GetXaxis()->SetLabelSize(0.045);
    fh1_mwpc_cal[0]->GetXaxis()->SetTitleSize(0.045);
    fh1_mwpc_cal[0]->GetYaxis()->SetLabelSize(0.045);
    fh1_mwpc_cal[0]->GetYaxis()->SetTitleSize(0.045);
    cMWPCCal->cd(1);
    fh1_mwpc_cal[0]->Draw("");

    Name1 = "fh1_" + fNameDet + "_posy_cal";
    Name2 = fNameDet + ": Position Y";

    if (fNameDet == "Mwpc0")
        fh1_mwpc_cal[1] = new TH1F(Name1, Name2, 64, 0.5, 64.5);
    else if (fNameDet == "Mwpc1" || fNameDet == "Mwpc2")
        fh1_mwpc_cal[1] = new TH1F(Name1, Name2, 40, 0.5, 40.5);
    else
        fh1_mwpc_cal[1] = new TH1F(Name1, Name2, 120, 0.5, 120.5);
    fh1_mwpc_cal[1]->GetXaxis()->SetTitle("Position Y [pads]");
    fh1_mwpc_cal[1]->GetYaxis()->SetTitle("Counts");
    fh1_mwpc_cal[1]->GetYaxis()->SetTitleOffset(1.15);
    fh1_mwpc_cal[1]->GetXaxis()->CenterTitle(true);
    fh1_mwpc_cal[1]->GetYaxis()->CenterTitle(true);
    fh1_mwpc_cal[1]->GetXaxis()->SetLabelSize(0.045);
    fh1_mwpc_cal[1]->GetXaxis()->SetTitleSize(0.045);
    fh1_mwpc_cal[1]->GetYaxis()->SetLabelSize(0.045);
    fh1_mwpc_cal[1]->GetYaxis()->SetTitleSize(0.045);
    cMWPCCal->cd(2);
    fh1_mwpc_cal[1]->Draw("");

    cMWPCCal2D = new TCanvas(fNameDet + "_cal2D", fNameDet + " cal 2D info", 10, 10, 800, 700);

    Name1 = "fh2_" + fNameDet + "_posXy_cal";
    Name2 = fNameDet + ": Position X vs Y";
    if (fNameDet == "Mwpc0")
        fh2_mwpc_cal = new TH2F(Name1, Name2, 64, 0.5, 64.5, 64, 0.5, 64.5);
    else if (fNameDet == "Mwpc1" || fNameDet == "Mwpc2")
        fh2_mwpc_cal = new TH2F(Name1, Name2, 64, 0.5, 64.5, 40, 0.5, 40.5);
    else
        fh2_mwpc_cal = new TH2F(Name1, Name2, 288, 0.5, 288.5, 120, 0.5, 120.5);
    fh2_mwpc_cal->GetXaxis()->SetTitle("Position X [pads]");
    fh2_mwpc_cal->GetYaxis()->SetTitle("Position Y [pads]");
    fh2_mwpc_cal->GetYaxis()->SetTitleOffset(1.1);
    fh2_mwpc_cal->GetXaxis()->CenterTitle(true);
    fh2_mwpc_cal->GetYaxis()->CenterTitle(true);
    fh2_mwpc_cal->GetXaxis()->SetLabelSize(0.045);
    fh2_mwpc_cal->GetXaxis()->SetTitleSize(0.045);
    fh2_mwpc_cal->GetYaxis()->SetLabelSize(0.045);
    fh2_mwpc_cal->GetYaxis()->SetTitleSize(0.045);
    fh2_mwpc_cal->Draw("col");

    TCanvas* cx = new TCanvas(fNameDet + "_XQ", "", 10, 10, 800, 700);
    Name1 = "fh2_" + fNameDet + "_XQ";
    Name2 = fNameDet + ": Pad vs XQ";

    if (fNameDet == "Mwpc0" || fNameDet == "Mwpc1" || fNameDet == "Mwpc2")
        fh2_mwpc_xq = new TH2F(Name1, Name2, 256, 0.5, 64.5, 2000, 0, 4000);
    else
        fh2_mwpc_xq = new TH2F(Name1, Name2, 288 * 4, 0.5, 288.5, 2000, 0, 4000);

    fh2_mwpc_xq->GetXaxis()->SetTitle("Pad number");
    fh2_mwpc_xq->GetYaxis()->SetTitle("Charge [channels]");
    fh2_mwpc_xq->GetYaxis()->SetTitleOffset(1.1);
    fh2_mwpc_xq->GetXaxis()->CenterTitle(true);
    fh2_mwpc_xq->GetYaxis()->CenterTitle(true);
    fh2_mwpc_xq->GetXaxis()->SetLabelSize(0.045);
    fh2_mwpc_xq->GetXaxis()->SetTitleSize(0.045);
    fh2_mwpc_xq->GetYaxis()->SetLabelSize(0.045);
    fh2_mwpc_xq->GetYaxis()->SetTitleSize(0.045);
    fh2_mwpc_xq->Draw("col");

    TCanvas* cy = new TCanvas(fNameDet + "_YQ", "", 10, 10, 800, 700);
    Name1 = "fh2_" + fNameDet + "_YQ";
    Name2 = fNameDet + ": Pad vs YQ";
    if (fNameDet == "Mwpc0")
        fh2_mwpc_yq = new TH2F(Name1, Name2, 256, 0.5, 64.5, 2000, 0, 4000);
    else if (fNameDet == "Mwpc1" || fNameDet == "Mwpc2")
        fh2_mwpc_yq = new TH2F(Name1, Name2, 40 * 4, 0.5, 40.5, 2000, 0, 4000);
    else
        fh2_mwpc_yq = new TH2F(Name1, Name2, 120 * 4, 0.5, 120.5, 2000, 0, 4000);

    fh2_mwpc_yq->GetXaxis()->SetTitle("Pad number");
    fh2_mwpc_yq->GetYaxis()->SetTitle("Charge [channels]");
    fh2_mwpc_yq->GetYaxis()->SetTitleOffset(1.1);
    fh2_mwpc_yq->GetXaxis()->CenterTitle(true);
    fh2_mwpc_yq->GetYaxis()->CenterTitle(true);
    fh2_mwpc_yq->GetXaxis()->SetLabelSize(0.045);
    fh2_mwpc_yq->GetXaxis()->SetTitleSize(0.045);
    fh2_mwpc_yq->GetYaxis()->SetLabelSize(0.045);
    fh2_mwpc_yq->GetYaxis()->SetTitleSize(0.045);
    fh2_mwpc_yq->Draw("col");

     // Hit data
    chitx = new TCanvas(fNameDet + "_Xpos", "", 10, 10, 800, 700);
    Name1 = "fh1_" + fNameDet + "_Xpos";
    Name2 = fNameDet + ": X (mm)";
    if (fNameDet == "Mwpc3")
        fh1_Xpos = new TH1F(Name1, Name2, 1800, -450, 450);
    else
        fh1_Xpos = new TH1F(Name1, Name2, 400, -100, 100);
    fh1_Xpos->GetXaxis()->SetTitle("X (mm)");
    fh1_Xpos->GetYaxis()->SetTitle("counts per bin");
    fh1_Xpos->GetYaxis()->SetTitleOffset(1.1);
    fh1_Xpos->GetXaxis()->CenterTitle(true);
    fh1_Xpos->GetYaxis()->CenterTitle(true);
    fh1_Xpos->GetXaxis()->SetLabelSize(0.045);
    fh1_Xpos->GetXaxis()->SetTitleSize(0.045);
    fh1_Xpos->GetYaxis()->SetLabelSize(0.045);
    fh1_Xpos->GetYaxis()->SetTitleSize(0.045);
    fh1_Xpos->Draw("col");

      // Hit data
    chity = new TCanvas(fNameDet + "_Ypos", "", 10, 10, 800, 700);
    Name1 = "fh1_" + fNameDet + "_Ypos";
    Name2 = fNameDet + ": Y (mm)";
    if (fNameDet == "Mwpc3")
        fh1_Ypos = new TH1F(Name1, Name2, 1200, -300, 300);
    else
        fh1_Ypos = new TH1F(Name1, Name2, 400, -100, 100);
    fh1_Ypos->GetXaxis()->SetTitle("Y (mm)");
    fh1_Ypos->GetYaxis()->SetTitle("counts per bin");
    fh1_Ypos->GetYaxis()->SetTitleOffset(1.1);
    fh1_Ypos->GetXaxis()->CenterTitle(true);
    fh1_Ypos->GetYaxis()->CenterTitle(true);
    fh1_Ypos->GetXaxis()->SetLabelSize(0.045);
    fh1_Ypos->GetXaxis()->SetTitleSize(0.045);
    fh1_Ypos->GetYaxis()->SetLabelSize(0.045);
    fh1_Ypos->GetYaxis()->SetTitleSize(0.045);
    fh1_Ypos->Draw("col");


    // Hit data
    chitxy = new TCanvas(fNameDet + "_XYpos", "", 10, 10, 800, 700);
    Name1 = "fh2_" + fNameDet + "_XYpos";
    Name2 = fNameDet + ": Y vs X (mm)";
    if (fNameDet == "Mwpc3")
        fh2_XYpos = new TH2F(Name1, Name2, 1800, -450, 450, 1200, -300, 300);
    else
        fh2_XYpos = new TH2F(Name1, Name2, 400, -100, 100, 400, -100, 100);
    fh2_XYpos->GetXaxis()->SetTitle("X (mm)");
    fh2_XYpos->GetYaxis()->SetTitle("Y (mm)");
    fh2_XYpos->GetYaxis()->SetTitleOffset(1.1);
    fh2_XYpos->GetXaxis()->CenterTitle(true);
    fh2_XYpos->GetYaxis()->CenterTitle(true);
    fh2_XYpos->GetXaxis()->SetLabelSize(0.045);
    fh2_XYpos->GetXaxis()->SetTitleSize(0.045);
    fh2_XYpos->GetYaxis()->SetLabelSize(0.045);
    fh2_XYpos->GetYaxis()->SetTitleSize(0.045);
    fh2_XYpos->Draw("col");

    // MAIN FOLDER-MWPC
    TFolder* mainfolMW = new TFolder(fNameDet, fNameDet + " info");
    mainfolMW->Add(cMWPCCal);
    mainfolMW->Add(cMWPCCal2D);
    mainfolMW->Add(cx);
    mainfolMW->Add(cy);
    if (fHitItemsMwpc){
        mainfolMW->Add(chitx);
        mainfolMW->Add(chity);
        mainfolMW->Add(chitxy);
    }
    run->AddObject(mainfolMW);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_" + fNameDet + "_HIST",
                                          Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BSofMwpcOnlineSpectra::Reset_Histo()
{
    LOG(INFO) << "R3BSof" + fNameDet + "OnlineSpectra::Reset_Histo";
    // Cal data
    if (fCalItemsMwpc)
    {
        for (Int_t i = 0; i < 2; i++)
            fh1_mwpc_cal[i]->Reset();
        fh2_mwpc_cal->Reset();
        fh2_mwpc_xq->Reset();
        fh2_mwpc_yq->Reset();
    }
    // Hit data
    if (fHitItemsMwpc)
    {
        fh1_Xpos->Reset();
        fh1_Ypos->Reset();
        fh2_XYpos->Reset();
    }
}

void R3BSofMwpcOnlineSpectra::Exec(Option_t* option)
{
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BSof" + fNameDet + "OnlineSpectra::Exec FairRootManager not found";

    // Fill Cal data
    if (fCalItemsMwpc && fCalItemsMwpc->GetEntriesFast() > 0)
    {
        Int_t nHits = fCalItemsMwpc->GetEntriesFast();
        Int_t maxpadx = -1, maxpady = -1, maxqx = 0, maxqy = 0;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BSofMwpcCalData* hit = (R3BSofMwpcCalData*)fCalItemsMwpc->At(ihit);
            if (!hit)
                continue;
            if (hit->GetPlane() == 1 || hit->GetPlane() == 2)
            {
                fh1_mwpc_cal[0]->Fill(hit->GetPad());
                fh2_mwpc_xq->Fill(hit->GetPad() + gRandom->Uniform(-0.5, 0.5), hit->GetQ());
                if (hit->GetQ() > maxqx)
                {
                    maxpadx = hit->GetPad();
                    maxqx = hit->GetQ();
                }
            }
            if (hit->GetPlane() == 3)
            {
                fh1_mwpc_cal[1]->Fill(hit->GetPad());
                fh2_mwpc_yq->Fill(hit->GetPad() + gRandom->Uniform(-0.5, 0.5), hit->GetQ());
                if (hit->GetQ() > maxqy)
                {
                    maxpady = hit->GetPad();
                    maxqy = hit->GetQ();
                }
            }
        }
        if (maxpadx > -1 && maxpady > -1)
            fh2_mwpc_cal->Fill(maxpadx, maxpady);
    }

    // Fill Hit data
    if (fHitItemsMwpc && fHitItemsMwpc->GetEntriesFast() > 0)
    {
        Int_t nHits = fHitItemsMwpc->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BSofMwpcHitData* hit = (R3BSofMwpcHitData*)fHitItemsMwpc->At(ihit);
            if (!hit)
                continue;
            fh1_Xpos->Fill(hit->GetX());
            fh1_Ypos->Fill(hit->GetY());
            fh2_XYpos->Fill(hit->GetX(), hit->GetY());
        }
    }

    fNEvents += 1;
}

void R3BSofMwpcOnlineSpectra::FinishEvent()
{

    if (fCalItemsMwpc)
    {
        fCalItemsMwpc->Clear();
    }
    if (fHitItemsMwpc)
    {
        fHitItemsMwpc->Clear();
    }
}

void R3BSofMwpcOnlineSpectra::FinishTask()
{

    if (fCalItemsMwpc)
    {
        cMWPCCal->Write();
        cMWPCCal2D->Write();
        fh2_mwpc_xq->Write();
        fh2_mwpc_yq->Write();
    }
    if (fHitItemsMwpc)
    {
        chitx->Write();
        chity->Write();
        chitxy->Write();
    }
}

ClassImp(R3BSofMwpcOnlineSpectra)
