/*
 *  Macro to run the online for all the detectors simultaneously
 *
 *  Author: Jose Luis <joseluis.rodriguez.sanchez@usc.es>
 *  @since Jan 6th, 2020
 *
 */

typedef struct EXT_STR_h101_t
{
    EXT_STR_h101_unpack_t unpack;
    EXT_STR_h101_SOFMWPC_onion_t mwpc;
    EXT_STR_h101_MUSIC_onion_t music;
    EXT_STR_h101_SOFSCI_onion_t sci;
    EXT_STR_h101_AMS_t ams;
    EXT_STR_h101_CALIFA_t califa;
    EXT_STR_h101_SOFTWIM_onion_t twim;
    EXT_STR_h101_SOFTOFW_onion_t tofw;
    EXT_STR_h101_raw_nnp_tamex_t raw_nnp;
} EXT_STR_h101;

void main_online()
{
    TStopwatch timer;
    timer.Start();

    // const Int_t nev = -1; // number of events to read, -1 - until CTRL+C
    const Int_t nev = -1; // Only nev events to read
    const Int_t fRunId = 1;

    // Create input -----------------------------------------
     TString filename = "--stream=lxir123:7803";
    //TString filename = "~/lmd/sofia2019/main0079_0001.lmd";
    // TString filename = "/media/audrey/COURGE/SOFIA/ANALYSE/SOFIA3/data/main0028_0001.lmd";

    // Output file ------------------------------------------
    TString outputFileName = "data_s444_online.root";
    Bool_t NOTstoremappeddata = false; // if true, don't store mapped data in the root file
    Bool_t NOTstorecaldata = false;    // if true, don't store cal data in the root file
    Bool_t NOTstorehitdata = false;    // if true, don't store hit data in the root file

    // Online server configuration --------------------------
    Int_t refresh = 1; // Refresh rate for online histograms
    Int_t port = 8888; // Port number for the online visualization, example lxgXXXX:8888

    // UCESB configuration ----------------------------------
    TString ntuple_options = "RAW";
    TString ucesb_dir = getenv("UCESB_DIR");
    TString ucesb_path = "/u/land/lynx.landexp/201911_eng/upexps/201911_eng2/201911_eng2 --allow-errors --input-buffer=100Mi";
    //TString ucesb_path = ucesb_dir + "/../upexps/201911_eng2/201911_eng2 --allow-errors --input-buffer=100Mi";
    ucesb_path.ReplaceAll("//", "/");

    // Setup: Selection of detectors ------------------------
    Bool_t fMwpc0 = true;    // MWPC0 for tracking at entrance of Cave-C
    Bool_t fMusic = true;    // R3B-Music: Ionization chamber for charge-Z
    Bool_t fSci = true;      // Start: Plastic scintillator for ToF
    Bool_t fAms = false;     // AMS tracking detectors
    Bool_t fCalifa = false;  // Califa calorimeter
    Bool_t fMwpc1 = false;   // MWPC1 for tracking of fragments in front of target
    Bool_t fMwpc2 = true;    // MWPC2 for tracking of fragments before GLAD
    Bool_t fTwim = true;     // Twim: Ionization chamber for charge-Z of fragments
    Bool_t fMwpc3 = true;    // MWPC3 for tracking of fragments behind GLAD
    Bool_t fTofW = true;     // ToF-Wall for time-of-flight of fragments behind GLAD
    Bool_t fNeuland = false; // NeuLAND for neutrons behind GLAD

    // Configuration of each detector -----------------------
    const Int_t NLnBarsPerPlane = 50; // NeuLAND: number of scintillator bars per plane
    const Int_t NLnPlanes = 16;       // NeuLAND: number of planes (for TCAL calibration)

    // Calibration files ------------------------------------
    TString caldir = gSystem->Getenv("VMCWORKDIR");
    // SOFIA detectors
    TString sofiacaldir = caldir + "/sofia/macros/s444/parameters/";
    TString sofiacalfilename = sofiacaldir + "CalibParam.par";
    sofiacalfilename.ReplaceAll("//", "/");

    // Create source using ucesb for input ------------------
    EXT_STR_h101 ucesb_struct;

    R3BUcesbSource* source =
        new R3BUcesbSource(filename, ntuple_options, ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
    source->SetMaxEvents(nev);

    // Definition of reader ---------------------------------
    R3BUnpackReader* unpackreader =
        new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct, offsetof(EXT_STR_h101, unpack));

    R3BMusicReader* unpackmusic;
    R3BSofSciReader* unpacksci;
    R3BAmsReader* unpackams;
    R3BCalifaFebexReader* unpackcalifa;
    R3BSofMwpcReader* unpackmwpc;
    R3BSofTwimReader* unpacktwim;
    R3BSofToFWReader* unpacktofw;
    R3BNeulandTamexReader* unpackneuland;

    if (fMusic)
        unpackmusic = new R3BMusicReader((EXT_STR_h101_MUSIC_t*)&ucesb_struct.music, offsetof(EXT_STR_h101, music));

    if (fSci)
        unpacksci = new R3BSofSciReader((EXT_STR_h101_SOFSCI_t*)&ucesb_struct.sci, offsetof(EXT_STR_h101, sci));

    if (fAms)
        unpackams = new R3BAmsReader((EXT_STR_h101_AMS*)&ucesb_struct.ams, offsetof(EXT_STR_h101, ams));

    if (fCalifa)
        unpackcalifa =
            new R3BCalifaFebexReader((EXT_STR_h101_CALIFA*)&ucesb_struct.califa, offsetof(EXT_STR_h101, califa));

    if (fMwpc0 || fMwpc1 || fMwpc2 || fMwpc3)
        unpackmwpc = new R3BSofMwpcReader((EXT_STR_h101_SOFMWPC_t*)&ucesb_struct.mwpc, offsetof(EXT_STR_h101, mwpc));

    if (fTwim)
        unpacktwim = new R3BSofTwimReader((EXT_STR_h101_SOFTWIM_t*)&ucesb_struct.twim, offsetof(EXT_STR_h101, twim));

    if (fTofW)
        unpacktofw = new R3BSofToFWReader((EXT_STR_h101_SOFTOFW_t*)&ucesb_struct.tofw, offsetof(EXT_STR_h101, tofw));

    if (fNeuland)
        unpackneuland = new R3BNeulandTamexReader((EXT_STR_h101_raw_nnp_tamex_t*)&ucesb_struct.raw_nnp,
                                                  offsetof(EXT_STR_h101, raw_nnp));

    // Add readers ------------------------------------------
    source->AddReader(unpackreader);
    if (fMusic)
    {
        unpackmusic->SetOnline(NOTstoremappeddata);
        source->AddReader(unpackmusic);
    }
    if (fSci)
    {
        unpacksci->SetOnline(NOTstoremappeddata);
        source->AddReader(unpacksci);
    }
    if (fMwpc0 || fMwpc1 || fMwpc2 || fMwpc3)
    {
        unpackmwpc->SetOnline(NOTstoremappeddata);
        source->AddReader(unpackmwpc);
    }
    if (fAms)
    {
        unpackams->SetOnline(NOTstoremappeddata);
        source->AddReader(unpackams);
    }
    if (fCalifa)
    {
        unpackcalifa->SetOnline(NOTstoremappeddata);
        source->AddReader(unpackcalifa);
    }
    if (fTwim)
    {
        unpacktwim->SetOnline(NOTstoremappeddata);
        source->AddReader(unpacktwim);
    }
    if (fTofW)
    {
        unpacktofw->SetOnline(NOTstoremappeddata);
        source->AddReader(unpacktofw);
    }
    if (fNeuland)
    {
        // unpackneuland->SetNofPlanes(NLnPlanes);
        source->AddReader(unpackneuland);
    }

    // Create online run ------------------------------------
    FairRunOnline* run = new FairRunOnline(source);
    run->SetRunId(fRunId);
    run->SetSink(new FairRootFileSink(outputFileName));
    run->ActivateHttpServer(refresh, port);

    // Runtime data base ------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();

    FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo(); // Ascii
    parIo1->open(sofiacalfilename, "in");
    rtdb->setFirstInput(parIo1);
    rtdb->print();

    // Add analysis task ------------------------------------
    // MWPC0
    if (fMwpc0)
    {
        R3BSofMwpc0Mapped2Cal* MW0Map2Cal = new R3BSofMwpc0Mapped2Cal();
        MW0Map2Cal->SetOnline(NOTstorecaldata);
        run->AddTask(MW0Map2Cal);

        R3BSofMwpc0Cal2Hit* MW0Cal2Hit = new R3BSofMwpc0Cal2Hit();
        MW0Cal2Hit->SetOnline(NOTstorehitdata);
        run->AddTask(MW0Cal2Hit);
    }

    // MUSIC
    if (fMusic)
    {
        R3BMusicMapped2Cal* MusMap2Cal = new R3BMusicMapped2Cal();
        MusMap2Cal->SetOnline(NOTstorecaldata);
        run->AddTask(MusMap2Cal);

        R3BMusicCal2Hit* MusCal2Hit = new R3BMusicCal2Hit();
        MusCal2Hit->SetOnline(NOTstorehitdata);
        run->AddTask(MusCal2Hit);
    }

    // SCI
    if (fSci)
    {
        // --- Mapped 2 Tcal for SofSci
        R3BSofSciMapped2Tcal* SofSciMap2Tcal = new R3BSofSciMapped2Tcal();
        run->AddTask(SofSciMap2Tcal);

        // --- Tcal 2 SingleTcal for SofSci
        R3BSofSciTcal2SingleTcal* SofSciTcal2STcal = new R3BSofSciTcal2SingleTcal();
        run->AddTask(SofSciTcal2STcal);
    }

    // AMS
    if (fAms)
    {
        R3BAmsMapped2StripCal* AmsMap2Cal = new R3BAmsMapped2StripCal();
        AmsMap2Cal->SetOnline(NOTstorecaldata);
        run->AddTask(AmsMap2Cal);
        R3BAmsStripCal2Hit* AmsCal2Hit = new R3BAmsStripCal2Hit();
        AmsCal2Hit->SetOnline(NOTstorehitdata);
        run->AddTask(AmsCal2Hit);
    }

    // CALIFA
    if (fCalifa)
    {
        // R3BCalifaMapped2CrystalCal
        R3BCalifaMapped2CrystalCal* CalifaMap2Cal = new R3BCalifaMapped2CrystalCal();
        CalifaMap2Cal->SetOnline(NOTstorecaldata);
        run->AddTask(CalifaMap2Cal);
        // R3BCalifaCrystalCal2Hit FIXME
        /*      R3BCalifaCrystalCal2Hit* CalifaCal2Hit = new R3BCalifaCrystalCal2Hit();
                CalifaCal2Hit->SelectGeometryVersion(444);
                CalifaCal2Hit->SetSquareWindowAlg(0.25, 0.25); //[0.25 around 14.3 degrees, 3.2 for the complete calorimeter] 
                CalifaCal2Hit->SetDetectionThreshold(200);     // 200 KeV CalifaCal2Hit->SetDRThreshold(15000);// 15 MeV 
                run->AddTask(CalifaCal2Hit);
        */
    }

    // MWPC1
    if (fMwpc1)
    {
        R3BSofMwpc1Mapped2Cal* MW1Map2Cal = new R3BSofMwpc1Mapped2Cal();
        MW1Map2Cal->SetOnline(NOTstorecaldata);
        run->AddTask(MW1Map2Cal);

        R3BSofMwpc1Cal2Hit* MW1Cal2Hit = new R3BSofMwpc1Cal2Hit();
        MW1Cal2Hit->SetOnline(NOTstorehitdata);
        run->AddTask(MW1Cal2Hit);
    }

    // TWIM
    if (fSci)
    {
        R3BSofTwimMapped2Cal* TwimMap2Cal = new R3BSofTwimMapped2Cal();
        run->AddTask(TwimMap2Cal);

        R3BSofTwimCal2Hit* TwimCal2Hit = new R3BSofTwimCal2Hit();
        run->AddTask(TwimCal2Hit);
    }

    // MWPC2
    if (fMwpc2)
    {
        R3BSofMwpc2Mapped2Cal* MW2Map2Cal = new R3BSofMwpc2Mapped2Cal();
        MW2Map2Cal->SetOnline(NOTstorecaldata);
        run->AddTask(MW2Map2Cal);

        R3BSofMwpc2Cal2Hit* MW2Cal2Hit = new R3BSofMwpc2Cal2Hit();
        MW2Cal2Hit->SetOnline(NOTstorehitdata);
        run->AddTask(MW2Cal2Hit);
    }

    // MWPC3
    if (fMwpc3)
    {
        R3BSofMwpc3Mapped2Cal* MW3Map2Cal = new R3BSofMwpc3Mapped2Cal();
        MW3Map2Cal->SetOnline(NOTstorecaldata);
        run->AddTask(MW3Map2Cal);

        R3BSofMwpc3Cal2Hit* MW3Cal2Hit = new R3BSofMwpc3Cal2Hit();
        MW3Cal2Hit->SetOnline(NOTstorehitdata);
        run->AddTask(MW3Cal2Hit);
    }

    // ToF-Wall
    if (fTofW)
    {
        // --- Mapped 2 Tcal for SofToFW
        R3BSofToFWMapped2Tcal* SofToFWMap2Tcal = new R3BSofToFWMapped2Tcal();
        run->AddTask(SofToFWMap2Tcal);
    }

    // Add online task ------------------------------------
    if (fMwpc0)
    {
        R3BSofMwpcOnlineSpectra* mw0online = new R3BSofMwpcOnlineSpectra("SofMwpc0OnlineSpectra", 1, "Mwpc0");
        run->AddTask(mw0online);
    }

    if (fMusic)
    {
        R3BMusicOnlineSpectra* musonline = new R3BMusicOnlineSpectra();
        run->AddTask(musonline);
    }

    if (fSci)
    {
        R3BSofSciOnlineSpectra* scionline = new R3BSofSciOnlineSpectra();
        run->AddTask(scionline);
    }

    if (fAms)
    {
        R3BAmsOnlineSpectra* AmsOnline = new R3BAmsOnlineSpectra();
        run->AddTask(AmsOnline);
    }

    if (fCalifa)
    {
        Int_t petals = 8; // FIXME with the last version!!
        R3BCalifaOnlineSpectra* CalifaOnline = new R3BCalifaOnlineSpectra();
        CalifaOnline->SetPetals(petals);
        CalifaOnline->SetRange_max(10000); // 10MeV
        run->AddTask(CalifaOnline);
    }

    if (fTwim)
    {
        R3BSofTwimOnlineSpectra* twonline = new R3BSofTwimOnlineSpectra();
        run->AddTask(twonline);
    }

    if (fMwpc1)
    {
        R3BSofMwpcOnlineSpectra* mw1online = new R3BSofMwpcOnlineSpectra("SofMwpc1OnlineSpectra", 1, "Mwpc1");
        run->AddTask(mw1online);
    }

    if (fMwpc0 && fMwpc1)
    {
        R3BSofMwpcCorrelationOnlineSpectra* mw0mw1online =
            new R3BSofMwpcCorrelationOnlineSpectra("SofMwpc0_1CorrelationOnlineSpectra", 1, "Mwpc0", "Mwpc1");
        run->AddTask(mw0mw1online);
    }

    if (fMwpc1 && fMwpc2)
    {
        R3BSofMwpcCorrelationOnlineSpectra* mw1mw2online =
            new R3BSofMwpcCorrelationOnlineSpectra("SofMwpc1_2CorrelationOnlineSpectra", 1, "Mwpc1", "Mwpc2");
        run->AddTask(mw1mw2online);
    }

    if (fMwpc2)
    {
        R3BSofMwpcOnlineSpectra* mw2online = new R3BSofMwpcOnlineSpectra("SofMwpc2OnlineSpectra", 1, "Mwpc2");
        run->AddTask(mw2online);
    }

    if (fMwpc0 && fMwpc2)
    {
        R3BSofMwpcCorrelationOnlineSpectra* mw0mw2online =
            new R3BSofMwpcCorrelationOnlineSpectra("SofMwpc0_2CorrelationOnlineSpectra", 1, "Mwpc0", "Mwpc2");
        run->AddTask(mw0mw2online);
    }

    if (fMwpc2 && fMwpc3)
    {
        R3BSofMwpcCorrelationOnlineSpectra* mw2mw3online =
            new R3BSofMwpcCorrelationOnlineSpectra("SofMwpc2_3CorrelationOnlineSpectra", 1, "Mwpc2", "Mwpc3");
        run->AddTask(mw2mw3online);
    }

    if (fMwpc3)
    {
        R3BSofMwpcOnlineSpectra* mw3online = new R3BSofMwpcOnlineSpectra("SofMwpc3OnlineSpectra", 1, "Mwpc3");
        run->AddTask(mw3online);
    }

    if (fTofW)
    {
        R3BSofToFWOnlineSpectra* tofwonline = new R3BSofToFWOnlineSpectra();
        run->AddTask(tofwonline);
    }

    R3BSofOnlineSpectra* sofonline = new R3BSofOnlineSpectra();
    run->AddTask(sofonline);

    // Initialize -------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");

    // Run --------------------------------------------------
    run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);

    // Finish -----------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    std::cout << std::endl << std::endl;
    std::cout << "Macro finished succesfully." << std::endl;
    std::cout << "Output file is " << outputFileName << std::endl;
    std::cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << std::endl << std::endl;
    // gApplication->Terminate();
}
