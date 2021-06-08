/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
  //기본 설정
  std::ios::sync_with_stdio(false);
  std::cout.tie(NULL);
  std::cout << "t " << query.GetNumVertices() << "\n";

  //체크할 순서 및 이웃 vertex list가 정리될 CS_beforeCheck
  std::vector< std::vector<uint32_t> > CS_beforeCheck(query.GetNumVertices());
  for(uint32_t i = 0; i < query.GetNumVertices(); i++) {
    std::vector<uint32_t> CS_tmp(1);
    CS_beforeCheck[i] = CS_tmp;
  }

  //현재 vertice의 어느 부분을 확인하는지 보여주는 CS_answer
  int64_t CS_answer[query.GetNumVertices()];
  std::vector<std::pair<uint32_t, uint32_t>> v;

  for(uint32_t j = 0; j < query.GetNumVertices(); j++) {
    CS_answer[j] = -1; //CS_answer initialize
    v.push_back(std::pair<uint32_t, uint32_t> (cs.GetCandidateSize(j), j));
  }

  std::sort(v.begin(), v.end());

  //v의 candidate 수에 따른 sorting 진행 후 이를 CS_beforeCheck에 저장
  for(uint32_t k = 0; k < query.GetNumVertices(); k++) {
    CS_beforeCheck[k][0] = v[k].second;
    for(uint32_t m = 0; m < k; m++) {
      if(query.IsNeighbor(CS_beforeCheck[m][0], CS_beforeCheck[k][0])) {
        CS_beforeCheck[k].push_back(CS_beforeCheck[m][0]);
      }
    }
  }

  /* debug용
  for(uint32_t k = 0; k < query.GetNumVertices(); k++) {
    std::cout << k << "&" << CS_beforeCheck[k][0] << ": ";
    for(uint32_t p = 1; p < CS_beforeCheck[k].size(); p++) {
      std::cout << CS_beforeCheck[k][p] << " ";
    }
    std::cout << "\n";
  }
  */

  int64_t loc_chk = 0; //현재 몇 번째 vertex를 살펴보는가
  uint32_t emb_cnt = 0; //출력된 graph 수
  while(loc_chk >= 0) {
    //n개의 vertex를 모두 select했을 시 해당 vertex 목록을 출력
    if(loc_chk == query.GetNumVertices()) {
      emb_cnt++;
      std::cout << "a ";
      for(uint32_t t = 0; t < query.GetNumVertices(); t++) {
        std::cout << cs.GetCandidate(t, CS_answer[t]) << " ";
      }
      std::cout << "\n";
      loc_chk--;
      if(emb_cnt == 100000) {
        break;
      }
      continue;
    }

    uint32_t ans_loc = CS_beforeCheck[loc_chk][0];
    CS_answer[ans_loc] += 1;

    //해당 vertex의 candidate 수보다 크게 잡힐 경우 reset
    if(CS_answer[ans_loc] >= cs.GetCandidateSize(ans_loc)) {
      CS_answer[ans_loc] = -1;
      loc_chk--;
      continue;
    }

    bool flag = true;
    //이미 해당 vertex를 선택했는지 여부를 확인
    for(uint32_t loc = 0; loc < loc_chk; loc++) {
      Vertex now_val = cs.GetCandidate(CS_beforeCheck[loc_chk][0], CS_answer[CS_beforeCheck[loc_chk][0]]);
      if(cs.GetCandidate(CS_beforeCheck[loc][0], CS_answer[CS_beforeCheck[loc][0]]) == now_val) {
        flag = false;
        break;
      }
    }

    if(!flag) {
      continue;
    } 

    //이미 선택된 vertex와 neighbor인지 확인
    for(uint32_t p = 1; p < CS_beforeCheck[loc_chk].size(); p++) {
      if(data.IsNeighbor(cs.GetCandidate(CS_beforeCheck[loc_chk][p], CS_answer[CS_beforeCheck[loc_chk][p]]), cs.GetCandidate(ans_loc, CS_answer[ans_loc])) == false) {
        flag = false;
        break;
      }
    }

    if(!flag) {
      continue;
    } else {
      loc_chk++;
    }
  }
}
