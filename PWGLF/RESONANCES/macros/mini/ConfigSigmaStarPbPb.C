//
// *** Configuration script for Sigma*->Lambda-Pi analysis with 2010 runs ***
// 
// A configuration script for RSN package needs to define the followings:
//
// (1) decay tree of each resonance to be studied, which is needed to select
//     true pairs and to assign the right mass to all candidate daughters
// (2) cuts at all levels: single daughters, tracks, events
// (3) output objects: histograms or trees
//
Bool_t ConfigSigmaStarPbPb
(  
   AliRsnMiniAnalysisTask *task,
   Bool_t		   isPP, 
   Bool_t                  isMC,
   Float_t                 piPIDCut,
   Float_t                 pPIDCut,
   Int_t                   aodFilterBit,
   Float_t                 trackDCAcut,
   Float_t                 massTol,
   Float_t                 lambdaDCA,
   Float_t                 lambdaCosPoinAn,
   Float_t                 lambdaDaughDCA,
   Int_t                   NTPCcluster,
   const char             *suffix,
   AliRsnCutSet           *cutsPair
)
{
   // manage suffix
   if (strlen(suffix) > 0) suffix = Form("_%s", suffix);
   
   // 
   // -- Define track cuts -------------------------------------------------------------------------
   //

   //TString s = ""; s+=trackDCAcut; s+="*(0.0026+0.0050/pt^1.01)";

   //const char *formula = s;

   // integrated pion cut
   AliRsnCutDaughterSigmaStar2010PP *cutPi = new AliRsnCutDaughterSigmaStar2010PP("cutPionForSigmaStar", AliPID::kPion);
   cutPi->SetPIDCut(piPIDCut);
   cutPi->SetMinTPCcluster(NTPCcluster);
   AliRsnCutTrackQuality *cutQuality = (AliRsnCutTrackQuality*) cutPi->CutQuality();
   cutQuality->SetAODTestFilterBit(aodFilterBit);
   //cutQuality->SetDCARPtFormula(formula);
   cutQuality->SetDCARmax(trackDCAcut);	         
    
   // cut set
   AliRsnCutSet *cutSetPi = new AliRsnCutSet("setPionForSigmaStar", AliRsnTarget::kDaughter);
   cutSetPi->AddCut(cutPi);
   cutSetPi->SetCutScheme(cutPi->GetName());
   // add to task
   Int_t iCutPi = task->AddTrackCuts(cutSetPi);
   
   // quality cuts
   AliESDtrackCuts *esdTrackCuts = new AliESDtrackCuts("qualityDaughterLambda");
   
   esdTrackCuts->SetAcceptKinkDaughters(0); // 0 = kFalse
   esdTrackCuts->SetMaxChi2PerClusterTPC(4);
   esdTrackCuts->SetMinNClustersTPC(NTPCcluster);
   esdTrackCuts->SetRequireTPCRefit();
   
   // cut lambda
   AliRsnCutV0 *cutLambda = new AliRsnCutV0("cutLambda", kLambda0, AliPID::kProton, AliPID::kPion);
   cutLambda->SetESDtrackCuts(esdTrackCuts);
   cutLambda->SetTolerance(massTol);
   cutLambda->SetMaxDCAVertex(lambdaDCA);
   cutLambda->SetMinCosPointingAngle(lambdaCosPoinAn);
   cutLambda->SetMaxDaughtersDCA(lambdaDaughDCA);
   cutLambda->SetMinTPCcluster(NTPCcluster);
   cutLambda->SetMaxRapidity(0.8);
   cutLambda->SetAODTestFilterBit(aodFilterBit);
   cutLambda->SetPIDCutProton(pPIDCut);
   cutLambda->SetPIDCutPion(piPIDCut);
   
   // cut set
   AliRsnCutSet *cutSetLambda = new AliRsnCutSet("setLambda", AliRsnTarget::kDaughter);
   cutSetLambda->AddCut(cutLambda);
   cutSetLambda->SetCutScheme(cutLambda->GetName());
   
   // add to task
   Int_t iCutLambda = task->AddTrackCuts(cutSetLambda);
   
   // cut anti-AntiLambda
   AliRsnCutV0 *cutAntiLambda = new AliRsnCutV0("cutAntiLambda", kLambda0Bar, AliPID::kProton, AliPID::kPion);
   cutAntiLambda->SetESDtrackCuts(esdTrackCuts);
   cutAntiLambda->SetTolerance(massTol);
   cutAntiLambda->SetMaxDCAVertex(lambdaDCA);
   cutAntiLambda->SetMinCosPointingAngle(lambdaCosPoinAn);
   cutAntiLambda->SetMaxDaughtersDCA(lambdaDaughDCA);
   cutAntiLambda->SetMinTPCcluster(NTPCcluster);
   cutAntiLambda->SetMaxRapidity(0.8);
   cutAntiLambda->SetAODTestFilterBit(aodFilterBit);
   cutAntiLambda->SetPIDCutProton(pPIDCut);
   cutAntiLambda->SetPIDCutPion(piPIDCut);
   
   // cut set
   AliRsnCutSet *cutSetAntiLambda = new AliRsnCutSet("setAntiLambda", AliRsnTarget::kDaughter);
   cutSetAntiLambda->AddCut(cutAntiLambda);
   cutSetAntiLambda->SetCutScheme(cutAntiLambda->GetName());
   // add to task
   Int_t iCutAntiLambda = task->AddTrackCuts(cutSetAntiLambda); 
   
   
   //######################################################################################################  
    
   //
   // -- Values ------------------------------------------------------------------------------------
   //
   
   /* invariant mass   */ Int_t imID   = task->CreateValue(AliRsnMiniValue::kInvMass, kFALSE);
   /* transv. momentum */ Int_t ptID   = task->CreateValue(AliRsnMiniValue::kPt, kFALSE);
   /* centrality       */ Int_t centID = task->CreateValue(AliRsnMiniValue::kMult, kFALSE);
   
   //
   // -- Create all needed outputs -----------------------------------------------------------------
   //
   
   // use an array for more compact writing, which are different on mixing and charges
   // [0] = unlike
   // [1] = mixing
   // [2] = like ++
   // [3] = like --
   Bool_t   use     [18] = { 1         ,  1         ,  1             ,  1             ,  1         ,  1         ,  1             ,  1             ,  1         ,  1         ,  1             ,  1             ,  1          ,  1              ,  1              ,  1              ,  1              ,  1              };
   Bool_t   useIM   [18] = { 1         ,  1         ,  1             ,  1             ,  1         ,  1         ,  1             ,  1             ,  1         ,  1         ,  1             ,  1             ,  1          ,  1              ,  1              ,  1              ,  1              ,  1              };
   TString  name    [18] = {"SigmaP"   , "SigmaM"   , "ASigmaP"      , "ASigmaM"      , "SigmaPmix", "SigmaMmix", "ASigmaPmix"   , "ASigmaMmix"   , "SigmaPt"  , "SigmaMt"  , "ASigmaPt"     , "ASigmaMt"     , "XiM"       , "XiP"           , "Lambda1520P"   , "Lambda1520M"   , "Lambda1520PBar", "Lambda1520MBar"};
   TString  comp    [18] = {"PAIR"     , "PAIR"     , "PAIR"         , "PAIR"         , "MIX"      , "MIX"      , "MIX"          , "MIX"          , "TRUE"     , "TRUE"     , "TRUE"         , "TRUE"         , "TRUE"      , "TRUE"          , "TRUE"          , "TRUE"          , "TRUE"          , "TRUE"          };
   TString  output  [18] = {"HIST"     , "HIST"     , "HIST"         , "HIST"         , "HIST"     , "HIST"     , "HIST"         , "HIST"         , "HIST"     , "HIST"     , "HIST"         , "HIST"         , "HIST"      , "HIST"          , "HIST"          , "HIST"          , "HIST"          , "HIST"          };
   Char_t   charge1 [18] = {'0'        , '0'        , '0'            , '0'            , '0'        , '0'        , '0'            , '0'            , '0'        , '0'        , '0'            , '0'            , '0'         , '0'             , '0'             , '0'             , '0'             , '0'             };
   Char_t   charge2 [18] = {'+'        , '-'        , '-'            , '+'            , '+'        , '-'        , '-'            , '+'            , '+'        , '-'        , '-'            , '+'            , '-'         , '+'             , '+'             , '-'             , '-'             , '+'             };
   Int_t    cutID1  [18] = { iCutLambda,  iCutLambda,  iCutAntiLambda,  iCutAntiLambda,  iCutLambda,  iCutLambda,  iCutAntiLambda,  iCutAntiLambda,  iCutLambda,  iCutLambda,  iCutAntiLambda,  iCutAntiLambda,  iCutLambda ,  iCutAntiLambda ,  iCutLambda     ,  iCutLambda     ,  iCutAntiLambda ,  iCutAntiLambda };
   Int_t    cutID2  [18] = { iCutPi    ,  iCutPi    ,  iCutPi        ,  iCutPi        ,  iCutPi    ,  iCutPi    ,  iCutPi        ,  iCutPi        ,  iCutPi    ,  iCutPi    ,  iCutPi        ,  iCutPi        ,  iCutPi     ,  iCutPi         ,  iCutPi         ,  iCutPi         ,  iCutPi         ,  iCutPi         };
   Int_t    ipdg    [18] = { 3224      ,  3114      , -3224          , -3114          ,  3224      ,  3114      , -3224          , -3114          ,  3224      ,  3114      , -3224          , -3114          ,  3312       , -3312           ,  3124           ,  3124           , -3124           , -3124           };
   Double_t mass    [18] = { 1.3828    ,  1.3872    ,  1.3828        ,  1.3872        ,  1.3828    ,  1.3872    ,  1.3828        ,  1.3872        ,  1.3828    ,  1.3872    ,  1.3828        ,  1.3872        ,  1.32171    ,  1.32171        ,  1.5195         ,  1.5195         ,  1.5195         ,  1.5195         };
   
   for (Int_t i = 0; i < 18; i++) {
      if (!use[i]) continue;
      if (!isPP) output[i] = "SPARSE";
      // create output
      AliRsnMiniOutput *out = task->CreateOutput(Form("sigmastar_%s%s", name[i].Data(), suffix), output[i].Data(), comp[i].Data());
      // selection settings
      out->SetCutID(0, cutID1[i]);
      out->SetCutID(1, cutID2[i]);
      out->SetDaughter(0, AliRsnDaughter::kLambda);
      out->SetDaughter(1, AliRsnDaughter::kPion);
      out->SetCharge(0, charge1[i]);
      out->SetCharge(1, charge2[i]);
      out->SetMotherPDG(ipdg[i]);
      out->SetMotherMass(mass[i]);
      // pair cuts
      out->SetPairCuts(cutsPair);
      // axis X: invmass
      if (useIM[i]) 
         out->AddAxis(imID, 800, 1.2, 2.0);
      // axis Y: transverse momentum
         out->AddAxis(ptID, 100, 0.0, 10.0);
	 
      if (!isPP) out->AddAxis(centID, 100, 0.0, 100.0);
      
    } 
    
   AddMonitorOutput_PionDCA(cutSetPi->GetMonitorOutput());
   AddMonitorOutput_PionPIDCut(cutSetPi->GetMonitorOutput());
   AddMonitorOutput_PionNTPC(cutSetPi->GetMonitorOutput());
   
   AddMonitorOutput_LambdaMass(cutSetLambda->GetMonitorOutput());
   AddMonitorOutput_LambdaDCA(cutSetLambda->GetMonitorOutput());
   AddMonitorOutput_LambdaDaughterDCA(cutSetLambda->GetMonitorOutput());
   AddMonitorOutput_LambdaCosPointAngle(cutSetLambda->GetMonitorOutput());
   AddMonitorOutput_LambdaProtonPID(cutSetLambda->GetMonitorOutput());
   AddMonitorOutput_LambdaPionPID(cutSetLambda->GetMonitorOutput());
   
   AddMonitorOutput_LambdaMass(cutSetAntiLambda->GetMonitorOutput());
   AddMonitorOutput_LambdaDCA(cutSetAntiLambda->GetMonitorOutput());
   AddMonitorOutput_LambdaDaughterDCA(cutSetAntiLambda->GetMonitorOutput());
   AddMonitorOutput_LambdaCosPointAngle(cutSetAntiLambda->GetMonitorOutput());
   AddMonitorOutput_LambdaAntiProtonPID(cutSetAntiLambda->GetMonitorOutput());
   AddMonitorOutput_LambdaAntiPionPID(cutSetAntiLambda->GetMonitorOutput());
    
   if (isMC) {
   
   TString mode = "HIST";
   if (!isPP) mode = "SPARSE";
   
   // create output
   AliRsnMiniOutput *out = task->CreateOutput(Form("sigmastarP_TrueMC%s", suffix), mode.Data(), "MOTHER");
   // selection settings
   out->SetDaughter(0, AliRsnDaughter::kLambda);
   out->SetDaughter(1, AliRsnDaughter::kPion);
   out->SetMotherPDG(3224);
   out->SetMotherMass(1.3828);
   // pair cuts
   out->SetPairCuts(cutsPair);
   // binnings
   out->AddAxis(imID, 800, 1.2, 2.0);
   out->AddAxis(ptID, 100, 0.0, 10.0);

   if (!isPP) out->AddAxis(centID, 100, 0.0, 100.0);
   
   // create output
   AliRsnMiniOutput *out = task->CreateOutput(Form("sigmastarM_TrueMC%s", suffix), mode.Data(), "MOTHER");
   // selection settings
   out->SetDaughter(0, AliRsnDaughter::kLambda);
   out->SetDaughter(1, AliRsnDaughter::kPion);
   out->SetMotherPDG(3114);
   out->SetMotherMass(1.3872);
   // pair cuts
   out->SetPairCuts(cutsPair);
   // binnings
   out->AddAxis(imID, 800, 1.2, 2.0);
   out->AddAxis(ptID, 100, 0.0, 10.0);

   if (!isPP) out->AddAxis(centID, 100, 0.0, 100.0);
   
   // create output
   AliRsnMiniOutput *out = task->CreateOutput(Form("sigmastarPBar_TrueMC%s", suffix), mode.Data(), "MOTHER");
   // selection settings
   out->SetDaughter(0, AliRsnDaughter::kLambda);
   out->SetDaughter(1, AliRsnDaughter::kPion);
   out->SetMotherPDG(-3224);
   out->SetMotherMass(1.3828);
   // pair cuts
   out->SetPairCuts(cutsPair);
   // binnings
   out->AddAxis(imID, 800, 1.2, 2.0);
   out->AddAxis(ptID, 100, 0.0, 10.0);

   if (!isPP) out->AddAxis(centID, 100, 0.0, 100.0);
   
   
   // create output
   AliRsnMiniOutput *out = task->CreateOutput(Form("sigmastarMBar_TrueMC%s", suffix), mode.Data(), "MOTHER");
   // selection settings
   out->SetDaughter(0, AliRsnDaughter::kLambda);
   out->SetDaughter(1, AliRsnDaughter::kPion);
   out->SetMotherPDG(-3114);
   out->SetMotherMass(1.3872);
   // pair cuts
   out->SetPairCuts(cutsPair);
   // binnings
   out->AddAxis(imID, 800, 1.2, 2.0);
   out->AddAxis(ptID, 100, 0.0, 10.0);

   if (!isPP) out->AddAxis(centID, 100, 0.0, 100.0);
   
   // create output
   AliRsnMiniOutput *out = task->CreateOutput(Form("XiP_TrueMC%s", suffix), mode.Data(), "MOTHER");
   // selection settings
   out->SetDaughter(0, AliRsnDaughter::kLambda);
   out->SetDaughter(1, AliRsnDaughter::kPion);
   out->SetMotherPDG(-3312);
   out->SetMotherMass(1.32171);
   // pair cuts
   out->SetPairCuts(cutsPair);
   // binnings
   out->AddAxis(imID, 800, 1.2, 2.0);
   out->AddAxis(ptID, 100, 0.0, 10.0);

   if (!isPP) out->AddAxis(centID, 100, 0.0, 100.0);
   
   
   // create output
   AliRsnMiniOutput *out = task->CreateOutput(Form("XiM_TrueMC%s", suffix), mode.Data(), "MOTHER");
   // selection settings
   out->SetDaughter(0, AliRsnDaughter::kLambda);
   out->SetDaughter(1, AliRsnDaughter::kPion);
   out->SetMotherPDG(3312);
   out->SetMotherMass(1.32171);
   // pair cuts
   out->SetPairCuts(cutsPair);
   // binnings
   out->AddAxis(imID, 800, 1.2, 2.0);
   out->AddAxis(ptID, 100, 0.0, 10.0);

   if (!isPP) out->AddAxis(centID, 100, 0.0, 100.0);
   
   
   AliRsnMiniOutput *out = task->CreateOutput(Form("Lambda1520P_TrueMC%s", suffix), mode.Data(), "MOTHER");
   // selection settings
   out->SetDaughter(0, AliRsnDaughter::kLambda);
   out->SetDaughter(1, AliRsnDaughter::kPion);
   out->SetCharge(0, 0);
   out->SetCharge(1, 1);
   out->SetMotherPDG(3124);
   out->SetMotherMass(1.5195);
   // pair cuts
   out->SetPairCuts(cutsPair);
   // binnings
   out->AddAxis(imID, 800, 1.2, 2.0);
   out->AddAxis(ptID, 100, 0.0, 10.0);

   if (!isPP) out->AddAxis(centID, 100, 0.0, 100.0);
   
   AliRsnMiniOutput *out = task->CreateOutput(Form("Lambda1520M_TrueMC%s", suffix), mode.Data(), "MOTHER");
   // selection settings
   out->SetDaughter(0, AliRsnDaughter::kLambda);
   out->SetDaughter(1, AliRsnDaughter::kPion);
   out->SetCharge(0, 0);
   out->SetCharge(1, -1);
   out->SetMotherPDG(3124);
   out->SetMotherMass(1.5195);
   // pair cuts
   out->SetPairCuts(cutsPair);
   // binnings
   out->AddAxis(imID, 800, 1.2, 2.0);
   out->AddAxis(ptID, 100, 0.0, 10.0);

   if (!isPP) out->AddAxis(centID, 100, 0.0, 100.0);
   
   
   // create output
   AliRsnMiniOutput *out = task->CreateOutput(Form("Lambda1520PBar_TrueMC%s", suffix), mode.Data(), "MOTHER");
   // selection settings
   out->SetDaughter(0, AliRsnDaughter::kLambda);
   out->SetDaughter(1, AliRsnDaughter::kPion);
   out->SetCharge(0, 0);
   out->SetCharge(1, 1);
   out->SetMotherPDG(-3124);
   out->SetMotherMass(1.5195);
   // pair cuts
   out->SetPairCuts(cutsPair);
   // binnings
   out->AddAxis(imID, 800, 1.2, 2.0);
   out->AddAxis(ptID, 100, 0.0, 10.0);

   if (!isPP) out->AddAxis(centID, 100, 0.0, 100.0);
   
   AliRsnMiniOutput *out = task->CreateOutput(Form("Lambda1520MBar_TrueMC%s", suffix), mode.Data(), "MOTHER");
   // selection settings
   out->SetDaughter(0, AliRsnDaughter::kLambda);
   out->SetDaughter(1, AliRsnDaughter::kPion);
   out->SetCharge(0, 0);
   out->SetCharge(1, -1);
   out->SetMotherPDG(-3124);
   out->SetMotherMass(1.5195);
   // pair cuts
   out->SetPairCuts(cutsPair);
   // binnings
   out->AddAxis(imID, 800, 1.2, 2.0);
   out->AddAxis(ptID, 100, 0.0, 10.0);

   if (!isPP) out->AddAxis(centID, 100, 0.0, 100.0);
   
   
   
   }

   return kTRUE;
}

void AddMonitorOutput_PionDCA(TObjArray *mon=0,TString opt="",AliRsnLoopDaughter *pdca=0)
{

   // PionDCA
   AliRsnValueDaughter *axisPionDCA = new AliRsnValueDaughter("pion_dca", AliRsnValueDaughter::kDCAXY);
   axisPionDCA->SetBins(0.0,1,0.001);

   // output: 2D histogram
   AliRsnListOutput *outMonitorPionDCA = new AliRsnListOutput("Pion_DCA", AliRsnListOutput::kHistoDefault);
   outMonitorPionDCA->AddValue(axisPionDCA);

   // add outputs to loop
   if (mon) mon->Add(outMonitorPionDCA);
   if (pdca) pdca->AddOutput(outMonitorPionDCA);
  
}

void AddMonitorOutput_PionPIDCut(TObjArray *mon=0,TString opt="",AliRsnLoopDaughter *piPID=0)
{

   // Pion PID Cut
   AliRsnValueDaughter *axisPionPIDCut = new AliRsnValueDaughter("pionPID", AliRsnValueDaughter::kTPCnsigmaPi);
   axisPionPIDCut->SetBins(0.0,5,0.01);

   // output: 2D histogram
   AliRsnListOutput *outMonitorPionPIDCut = new AliRsnListOutput("Pion_PID_Cut", AliRsnListOutput::kHistoDefault);
   outMonitorPionPIDCut->AddValue(axisPionPIDCut);

   // add outputs to loop
   if (mon) mon->Add(outMonitorPionPIDCut);
   if (piPID) piPID->AddOutput(outMonitorPionPIDCut);
  
}

void AddMonitorOutput_PionNTPC(TObjArray *mon=0,TString opt="",AliRsnLoopDaughter *piNTPC=0)
{

   // Pion PID Cut
   AliRsnValueDaughter *axisPionNTPC = new AliRsnValueDaughter("pionNTPC", AliRsnValueDaughter::kNTPCclusters);
   axisPionNTPC->SetBins(0.0,200,1);

   // output: 2D histogram
   AliRsnListOutput *outMonitorPionNTPC = new AliRsnListOutput("Pion_NTPC", AliRsnListOutput::kHistoDefault);
   outMonitorPionNTPC->AddValue(axisPionNTPC);

   // add outputs to loop
   if (mon) mon->Add(outMonitorPionNTPC);
   if (piNTPC) pNTPC->AddOutput(outMonitorPionNTPC);
  
}


void AddMonitorOutput_LambdaMass(TObjArray *mon=0,TString opt="",AliRsnLoopDaughter *lm=0)
{

   // Mass
   AliRsnValueDaughter *axisMass = new AliRsnValueDaughter("lambda_mass", AliRsnValueDaughter::kMass);
   axisMass->SetBins(0.7,1.5,0.001);

   // output: 2D histogram
   AliRsnListOutput *outMonitorM = new AliRsnListOutput("Lambda_Mass", AliRsnListOutput::kHistoDefault);
   outMonitorM->AddValue(axisMass);

   // add outputs to loop
   if (mon) mon->Add(outMonitorM);
   if (lm) lm->AddOutput(outMonitorM);
  
}

void AddMonitorOutput_LambdaDCA(TObjArray *mon=0,TString opt="",AliRsnLoopDaughter *ldca=0)
{

   // Lambda DCA
   AliRsnValueDaughter *axisLambdaDCA = new AliRsnValueDaughter("lambda_dca", AliRsnValueDaughter::kV0DCA);
   axisLambdaDCA->SetBins(0.0,1,0.001);

   // output: 2D histogram
   AliRsnListOutput *outMonitorLambdaDCA = new AliRsnListOutput("Lambda_DCA", AliRsnListOutput::kHistoDefault);
   outMonitorLambdaDCA->AddValue(axisLambdaDCA);

   // add outputs to loop
   if (mon) mon->Add(outMonitorLambdaDCA);
   if (ldca) ldca->AddOutput(outMonitorLambdaDCA);
  
}

void AddMonitorOutput_LambdaDaughterDCA(TObjArray *mon=0,TString opt="",AliRsnLoopDaughter *ldaugdca=0)
{

   // Lambda Daughter DCA
   AliRsnValueDaughter *axisLambdaDDCA = new AliRsnValueDaughter("lambda_daughterDCA", AliRsnValueDaughter::kDaughterDCA);
   axisLambdaDDCA->SetBins(0.0,1,0.001);

   // output: 2D histogram
   AliRsnListOutput *outMonitorLambdaDDCA = new AliRsnListOutput("Lambda_DaughterDCA", AliRsnListOutput::kHistoDefault);
   outMonitorLambdaDDCA->AddValue(axisLambdaDDCA);

   // add outputs to loop
   if (mon) mon->Add(outMonitorLambdaDDCA);
   if (ldaugdca) ldaugdca->AddOutput(outMonitorLambdaDDCA);
  
}

void AddMonitorOutput_LambdaCosPointAngle(TObjArray *mon=0,TString opt="",AliRsnLoopDaughter *lcpa=0)
{

   // Lambda Cosine of the Pointing Angle
   AliRsnValueDaughter *axisLambdaCPA = new AliRsnValueDaughter("lambda_cospointang", AliRsnValueDaughter::kCosPointAng);
   axisLambdaCPA->SetBins(0.97,1,0.0001);

   // output: 2D histogram
   AliRsnListOutput *outMonitorLambdaCPA = new AliRsnListOutput("Lambda_CosineOfPointingAngle", AliRsnListOutput::kHistoDefault);
   outMonitorLambdaCPA->AddValue(axisLambdaCPA);

   // add outputs to loop
   if (mon) mon->Add(outMonitorLambdaCPA);
   if (lcpa) lcpa->AddOutput(outMonitorLambdaCPA);
  
}

void AddMonitorOutput_LambdaProtonPID(TObjArray *mon=0,TString opt="",AliRsnLoopDaughter *lpPID=0)
{

   // Lambda Cosine of the Pointing Angle
   AliRsnValueDaughter *axisLambdaProtonPID = new AliRsnValueDaughter("lambda_protonPID", AliRsnValueDaughter::kLambdaProtonPIDCut);
   axisLambdaProtonPID->SetBins(0.0,5,0.01);

   // output: 2D histogram
   AliRsnListOutput *outMonitorLambdaProtonPID = new AliRsnListOutput("Lambda_ProtonPID", AliRsnListOutput::kHistoDefault);
   outMonitorLambdaProtonPID->AddValue(axisLambdaProtonPID);

   // add outputs to loop
   if (mon) mon->Add(outMonitorLambdaProtonPID);
   if (lpPID) lpPID->AddOutput(outMonitorLambdaProtonPID);
  
}

void AddMonitorOutput_LambdaPionPID(TObjArray *mon=0,TString opt="",AliRsnLoopDaughter *lpiPID=0)
{

   // Lambda Cosine of the Pointing Angle
   AliRsnValueDaughter *axisLambdaPionPID = new AliRsnValueDaughter("lambda_pionPID", AliRsnValueDaughter::kLambdaPionPIDCut);
   axisLambdaPionPID->SetBins(0.0,5,0.01);

   // output: 2D histogram
   AliRsnListOutput *outMonitorLambdaPionPID = new AliRsnListOutput("Lambda_PionPID", AliRsnListOutput::kHistoDefault);
   outMonitorLambdaPionPID->AddValue(axisLambdaPionPID);

   // add outputs to loop
   if (mon) mon->Add(outMonitorLambdaPionPID);
   if (lpiPID) lpiPID->AddOutput(outMonitorLambdaPionPID);
  
}

void AddMonitorOutput_LambdaAntiProtonPID(TObjArray *mon=0,TString opt="",AliRsnLoopDaughter *lapPID=0)
{

   // Lambda Cosine of the Pointing Angle
   AliRsnValueDaughter *axisLambdaAntiProtonPID = new AliRsnValueDaughter("lambda_antiprotonPID", AliRsnValueDaughter::kAntiLambdaAntiProtonPIDCut);
   axisLambdaAntiProtonPID->SetBins(0.0,5,0.01);

   // output: 2D histogram
   AliRsnListOutput *outMonitorLambdaAntiProtonPID = new AliRsnListOutput("Lambda_AntiProtonPID", AliRsnListOutput::kHistoDefault);
   outMonitorLambdaAntiProtonPID->AddValue(axisLambdaAntiProtonPID);

   // add outputs to loop
   if (mon) mon->Add(outMonitorLambdaAntiProtonPID);
   if (lapPID) lapPID->AddOutput(outMonitorLambdaAntiProtonPID);
  
}

void AddMonitorOutput_LambdaAntiPionPID(TObjArray *mon=0,TString opt="",AliRsnLoopDaughter *lapiPID=0)
{

   // Lambda Cosine of the Pointing Angle
   AliRsnValueDaughter *axisLambdaAntiPionPID = new AliRsnValueDaughter("lambda_antipionPID", AliRsnValueDaughter::kAntiLambdaAntiPionPIDCut);
   axisLambdaAntiPionPID->SetBins(0.0,5,0.01);

   // output: 2D histogram
   AliRsnListOutput *outMonitorLambdaAntiPionPID = new AliRsnListOutput("Lambda_AntiPionPID", AliRsnListOutput::kHistoDefault);
   outMonitorLambdaAntiPionPID->AddValue(axisLambdaAntiPionPID);

   // add outputs to loop
   if (mon) mon->Add(outMonitorLambdaAntiPionPID);
   if (lapiPID) lpiPID->AddOutput(outMonitorLambdaAntiPionPID);
  
}
