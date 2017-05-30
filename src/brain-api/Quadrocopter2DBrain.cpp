//
//  Quadrocopter2DBrain.cpp
//  Quadrocopter2DBrain
//
//  Created by anton on 29/04/16.
//  Copyright © 2016 anton. All rights reserved.
//

#include "Quadrocopter2DBrain.hpp"
#include <iostream>
#include "RLServerClient.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "main.h"
#include "ConsumerProducerSyncronizer.hpp"

namespace Quadrocopter2DBrain {

  const int actionSize = 2;

  RLServerClient rlClient;
  ConsumerProducerSyncronizer actSync (quadrocoptersCount);
  ConsumerProducerSyncronizer storeExpSync (quadrocoptersCount);
  std::vector<std::vector<float>> states;
  std::vector<std::vector<float>> acts;
  std::mutex mtxAct;
  std::mutex mtxStore;
  std::default_random_engine randomGenerator;
  std::vector<double> randomnessOfQuadrocopter;

	long quadrocopterBrainAct(
		int quadrocopterId,
		const std::vector<float>& state
	) {
		return 0;
	}

  //this function will be invoked from angent thead
  //so we can stop this thread until we have answer for him
	void quadrocopterBrainActCont(
		int quadrocopterId,
		const std::vector<float>& state,
		std::vector<float>& action
	) {
    mtxAct.lock();
    states [quadrocopterId] = state;
    mtxAct.unlock();
    actSync.reportProducerDone(quadrocopterId);
    actSync.waitConsumer(quadrocopterId);

    std::normal_distribution<float> normalNoise (0, randomnessOfQuadrocopter[quadrocopterId] * 2);
    mtxAct.lock();
    action [0] = acts [quadrocopterId][0] + normalNoise (randomGenerator);
    action [1] = acts [quadrocopterId][1] + normalNoise (randomGenerator);
    mtxAct.unlock();
//    if (quadrocopterId==0) {
//      std::cout << "--- before: " << acts [quadrocopterId][0] << " : " << acts [quadrocopterId][1] << std::endl;
//    }
  }

	void quadrocopterBrainActContLSTMWeak(
		int quadrocopterId,
		const std::vector<float>& state,
		std::vector<float>& action
	) {
	}
	
	void quadrocopterBrainActContMLPSeq(
		int quadrocopterId,
		const std::vector<float>& state,
		std::vector<float>& action
	) {
	}

	void storeQuadrocopterExperience (
		int quadrocopterId,
		double reward,
		long action,
		const std::vector <float>& prevState,
		const std::vector <float>& nextState
	) {
	}

  std::vector<bool> reseted;
  std::vector<double> rewards;
  std::vector<std::vector<float>> actions_exp;
  std::vector<std::vector<float>> prevStates;
  std::vector<std::vector<float>> nextStates;
  int stored = 0;
  int proceeded = 0;

  void storeQuadrocopterExperienceCont (
		int quadrocopterId,
		double reward,
		std::vector<float>& action,
		const std::vector <float>& prevState,
		const std::vector <float>& nextState
	) {
    mtxStore.lock();
    reseted [quadrocopterId] = false;
    rewards [quadrocopterId] = reward;
    actions_exp [quadrocopterId] = action;
    prevStates [quadrocopterId] = prevState;
    nextStates [quadrocopterId] = nextState;
    mtxStore.unlock();

    storeExpSync.reportProducerDone(quadrocopterId);
    storeExpSync.waitConsumer(quadrocopterId);
  }

  void resetStoreQuadrocopterExperienceCont (
    int quadrocopterId
  ) {
    mtxStore.lock();
    reseted [quadrocopterId] = true;
    mtxStore.unlock();

    storeExpSync.reportProducerDone(quadrocopterId);
    storeExpSync.waitConsumer(quadrocopterId);
  }

	void storeQuadrocopterExperienceContLSTMWeak (
		int quadrocopterId,
		double reward,
		std::vector<float>& action,
		const std::vector <float>& prevState,
		const std::vector <float>& nextState
	) {
	}

	void quadrocopterBrainActContLSTM(
		int quadrocopterId,
		const std::vector<float>& state,
		std::vector<float>& action
	) {
	}

	void storeQuadrocopterExperienceContLSTM (
		int quadrocopterId,
		double reward,
		std::vector<float>& action,
		const std::vector <float>& prevState,
		const std::vector <float>& nextState
	) {
	}

	void storeQuadrocopterExperienceContMLPSeq (
		int quadrocopterId,
		double reward,
		std::vector<float>& action,
		const std::vector <float>& prevState,
		const std::vector <float>& nextState
	) {
	}

	void initApiDiscreteDeepQ () {
    rlClient.connect ();

    states.resize(quadrocoptersCount);
    acts.resize(quadrocoptersCount);
    for (auto& a : acts) {
      a.resize (actionSize);
    }

    reseted.resize(quadrocoptersCount);
    rewards.resize(quadrocoptersCount);
    actions_exp.resize(quadrocoptersCount);
    prevStates.resize(quadrocoptersCount);
    nextStates.resize(quadrocoptersCount);

    randomnessOfQuadrocopter.resize(quadrocoptersCount);
    for (int i=0; i<quadrocoptersCount; i++) {
      if (i < 2) {
        randomnessOfQuadrocopter [i] = 0.1;
      } else
      if (i < 10) {
        randomnessOfQuadrocopter [i] = 0.05;
      }
      else {
        randomnessOfQuadrocopter [i] = 0.0;
      }
    }

    auto actThread = std::thread([](){

      while (true) {
        actSync.waitProducers();
        std::string received;

        rapidjson::Document d;
        d.SetObject();

        rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

        rapidjson::Value statesV (rapidjson::kArrayType);
        for (auto& state : states) {

          rapidjson::Value stateV (rapidjson::kArrayType);
          for (auto& s : state) {
            stateV.PushBack(s, allocator);
          }
          statesV.PushBack(stateV.Move(), allocator);

        }


        d.AddMember("method", rapidjson::Value("act_batch").Move(), allocator );
        d.AddMember("states", statesV.Move(), allocator);

        rapidjson::StringBuffer buffer;
        buffer.Clear();
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);
        std::string jsonStr (buffer.GetString());

        rlClient.send(jsonStr, received);
        rapidjson::Document receivedDoc;
        receivedDoc.Parse(received.c_str(), received.length());

        for (int i=0; i<quadrocoptersCount; i++) {
          auto& action = acts [i];
          action[0] = receivedDoc [i][0].GetDouble();
          action[1] = receivedDoc [i][1].GetDouble();
        }

        stored = 0;
        proceeded = 0;
        actSync.reportConsumerDone();
      }
    });

    actThread.detach();

    auto storeExpThread = std::thread([](){

      const int storeEveryNth = 5;
      int storeIndex = 0;
      while (true) {
        storeExpSync.waitProducers();

        storeIndex++;
        if (storeIndex % storeEveryNth != 0) {
          storeExpSync.reportConsumerDone();
          continue;
        }

        std::string received;

        rapidjson::Document d;
        d.SetObject();

        rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

        rapidjson::Value rewardsV (rapidjson::kArrayType);
        int i;
        i=0;
        for (auto& r : rewards) {
          if (reseted [i++]) continue;
          rewardsV.PushBack(r, allocator);
        }

        rapidjson::Value actionsV (rapidjson::kArrayType);
        i=0;
        for (auto& action : actions_exp) {
          if (reseted [i++]) continue;

          rapidjson::Value actV (rapidjson::kArrayType);
          for (auto& a : action) {
            actV.PushBack(a, allocator);
          }
          actionsV.PushBack(actV.Move(), allocator);
        }

        rapidjson::Value prevStatesV (rapidjson::kArrayType);
        i=0;
        for (auto& state : prevStates) {
          if (reseted [i++]) continue;

          rapidjson::Value stateV (rapidjson::kArrayType);
          for (auto& s : state) {
            stateV.PushBack(s, allocator);
          }
          prevStatesV.PushBack(stateV.Move(), allocator);
        }

        rapidjson::Value nextStatesV (rapidjson::kArrayType);
        i=0;
        for (auto& state : nextStates) {
          if (reseted [i++]) continue;

          rapidjson::Value stateV (rapidjson::kArrayType);
          for (auto& s : state) {
            stateV.PushBack(s, allocator);
          }
          nextStatesV.PushBack(stateV.Move(), allocator);
        }

        if (rewardsV.Size() != 0) {

          d.AddMember("method", rapidjson::Value("store_exp_batch").Move(), allocator );
          d.AddMember("rewards", rewardsV.Move(), allocator);
          d.AddMember("actions", actionsV.Move(), allocator);
          d.AddMember("prev_states", prevStatesV.Move(), allocator);
          d.AddMember("next_states", nextStatesV.Move(), allocator);

          rapidjson::StringBuffer buffer;
          buffer.Clear();
          rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
          d.Accept(writer);
          std::string jsonStr (buffer.GetString());

          rlClient.send(jsonStr, received);

        }

        storeExpSync.reportConsumerDone();
      }
    });

    storeExpThread.detach();
  }

	bool quadrocopterBrainTrain () {
		return true;
	}
	
	bool getBigErrorExp (
		std::vector <float>& state
	) {
		return true;
	}

	void resetQuadrocopterLSTMWeak (
		int quadrocopterId,
		const std::vector <float>& copterState
	) {
	}
	
	void resetQuadrocopterMLPSeq (
		int quadrocopterId,
		const std::vector <float>& copterState
	) {
	}
	
	void resetQuadrocopterLSTM (
		int quadrocopterId
	) {
	}
}
