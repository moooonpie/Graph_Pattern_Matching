/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
  /**
  * @brief C++의 출력을 빠르게 해주는 기본 설정
  */
  std::ios::sync_with_stdio(false);
  std::cout.tie(NULL);
  std::cout << "t " << query.GetNumVertices() << "\n";

  /**
  * @brief backtracking 순서 및 해당 vertex의 이웃들 list가 정리될 CS_beforeCheck에 대한 initialization
  */
  std::vector< std::vector<uint32_t> > CS_beforeCheck(query.GetNumVertices());
  for(uint32_t i = 0; i < query.GetNumVertices(); i++) {
    std::vector<uint32_t> CS_tmp(1);
    CS_beforeCheck[i] = CS_tmp;
  }

  /**
  * @brief 각 vertex의 CandidateSet 중 어느 부분을 searching하는지 보여주는 CS_answer 정의 및 initialization
  *        주어진 vertice 중 CandidateSet의 수가 가장 적은 vertex를 찾기 위한 minCSVertexIndex & minCSNum
  */
  int64_t CS_answer[query.GetNumVertices()];
  int minCSVertexIndex = -1;
  uint32_t minCSNum = 100000;

  //candidateSet 수가 제일 적은 vertex를 minCSVertexIndex에 저장
  for(uint32_t j = 0; j < query.GetNumVertices(); j++) {
    CS_answer[j] = -1; //CS_answer initialize
    if(cs.GetCandidateSize(j) < minCSNum) {
      minCSVertexIndex = j;
      minCSNum = cs.GetCandidateSize(j);
    }
  }

  /**
  * @brief vertex의 순서 정리를 위한 CS_orderCheck array initialization
  *        2: 이미 선정된 vertex, 1: 선정된 vertex의 neighbor, 0: 해당 없음 을 저장해 vertex 탐방 순서를 정한다.
  */
  int CS_orderCheck[query.GetNumVertices()] = {0};

  CS_orderCheck[minCSVertexIndex] = 2;
  CS_beforeCheck[0][0] = minCSVertexIndex;
  
  /**
  * @brief 정해진 vertex의 neighbor에 대해 CS_orderCheck의 값을 1로 변경
  */
  for(uint32_t tmp = query.GetNeighborStartOffset(minCSVertexIndex); tmp < query.GetNeighborEndOffset(minCSVertexIndex); tmp++) {
    CS_orderCheck[query.GetNeighbor(tmp)] = 1;
  }

  /* for debugging
  for(uint32_t j = 0; j < query.GetNumVertices(); j++) {
    std::cout << CS_orderCheck[j] << " ";
  }
  std::cout << "\n";
  */

  /**
  * @brief 현재까지 선택된 vertex들의 neighbor 기준으로 가장 CS가 적은 vertex를 선택
  *        선택된 vertex에 neighbor 검사가 진행되어야 하는 vertex의 index를 CS_beforeCheck의 뒤로 push_back
  */
  for(uint32_t k = 1; k < query.GetNumVertices(); k++) {
    minCSVertexIndex = -1;
    minCSNum = 100000;

    for(uint32_t j = 0; j < query.GetNumVertices(); j++) {
      if(CS_orderCheck[j] == 1 && cs.GetCandidateSize(j) < minCSNum) {
        minCSVertexIndex = j;
        minCSNum = cs.GetCandidateSize(j);
      }
    }

    CS_beforeCheck[k][0] = minCSVertexIndex;
    CS_orderCheck[minCSVertexIndex] = 2;
    for(uint32_t tmp = query.GetNeighborStartOffset(minCSVertexIndex); tmp < query.GetNeighborEndOffset(minCSVertexIndex); tmp++) {
      if(CS_orderCheck[query.GetNeighbor(tmp)] == 0) {
        CS_orderCheck[query.GetNeighbor(tmp)] = 1;
      }
    }

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
  
  /**
  * @brief 하단부터 backtracking 진행
  */
  while(loc_chk >= 0) {
    //std::cout << loc_chk << "\n"; //for debugging
    
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
