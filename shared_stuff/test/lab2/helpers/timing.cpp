#include "timing.h"


my_timers cur_timers;

const char time_unit_str[4][4] = {
  "s",
  "ms",
  "us",
  "ns"
};
unsigned long to_nsecs(struct timespec t) {
  return (t.tv_sec * ns_per_sec + t.tv_nsec);
}

unsigned long ns_diff(struct timespec t1, struct timespec t2) {
  return (to_nsecs(t1) - to_nsecs(t2));
}


unsigned long to_usecs(struct timespec t) {
  return to_nsecs(t)/unit_change;
}

unsigned long us_diff(struct timespec t1, struct timespec t2) {
  return (to_usecs(t1) - to_usecs(t2));
}


unsigned long to_msecs(struct timespec t) {
  return to_nsecs(t)/(unit_change*unit_change);
}

unsigned long ms_diff(struct timespec t1, struct timespec t2) {
  return (to_msecs(t1) - to_msecs(t2));
}


unsigned long to_secs(struct timespec t) {
  return to_nsecs(t)/(unit_change*unit_change*unit_change);
}

unsigned long s_diff(struct timespec t1, struct timespec t2) {
  return (to_secs(t1) - to_secs(t2));
}

double unit_convert(double time_ns, enum time_unit desired){
  double conversion = (((double)ns_per_sec)/(double)desired);
  return time_ns/conversion;
}

const char* unit_to_str(enum time_unit u){
  int index = 0;
  unsigned long s = u;
  while(s/unit_change){
    s = s/unit_change;
    index++;
  }
  return time_unit_str[index];
}


void printTimeHR(unsigned long nsecs){
  double exec_time = ((double)nsecs)/ns_per_sec;
  enum time_unit u = s;
  if(exec_time < human_readable_threshold){
    exec_time*=1000.0;
    u = ms;
  }
  if(exec_time < human_readable_threshold){
    exec_time*=1000.0;
    u = us;
  }
  if(exec_time < human_readable_threshold){
    exec_time*=1000.0;
    u = ns;
  }
  if(exec_time < human_readable_threshold){
    die("almost definetly an error in timing\n");
    return;
  }
  if(cur_timers.outfile){
    fprintf(cur_timers.outfile, "%.3lf %s\n", exec_time, unit_to_str(u));
  }
  else{
    fprintf(stdout, "%.3lf %s\n", exec_time, unit_to_str(u));
  }
}


struct timespec grabTime(){
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return t;
}


void initTiming(int nevents, int* ntimers, char*** dif_headers, char** ev_headers, FILE* outfile){
#ifdef USAGE_CHECK
  if(!nevents){
    die("nevents not specified: %d\n", nevents);
  }
#endif
  cur_timers.outfile = stdout;
  if(outfile){
    cur_timers.outfile = outfile;
  }
#ifdef med_verbose
    fprintf(stderr, "set outfile\n");
#endif
        
  cur_timers.ev_headers = ev_headers;
  cur_timers.nevents = nevents;
  cur_timers.events = (tevent *)mycalloc(nevents, sizeof(tevent));
  for(int i=0;i<nevents;i++){
    if(!ntimers || !ntimers[i]){
      cur_timers.events[i].ntimers = INT_MAX;
      if(dif_headers){
	cur_timers.events[i].headers = dif_headers[i];
      }
	cur_timers.events[i].times_nobound =
	  (struct timespec **)mycalloc(MAX_TIME_ARRAYS, sizeof(struct timespec*));
	cur_timers.events[i].times_nobound[0] =
	  (struct timespec *)mycalloc((1<<(DEFAULT_MIN_LOG_MINUS_ONE+1)),
				      sizeof(struct timespec));
	cur_timers.events[i].is_bounded = 0;
    }
    else{
    cur_timers.events[i].ntimers = ntimers[i];
    if(dif_headers){
      cur_timers.events[i].headers = dif_headers[i];
    }
    cur_timers.events[i].times = (struct timespec*)mycalloc(ntimers[i], sizeof(struct timespec));
    cur_timers.events[i].is_bounded = 1;
    }
  }

#ifdef med_verbose
    fprintf(stderr, "new timer: %d event(s) -> [", nevents);
    for(int i=0;i<nevents-1;i++){
      fprintf(stderr, "%d,", cur_timers.events[i].ntimers);
    }
    fprintf(stderr, "%d] indexes\n", cur_timers.events[nevents-1].ntimers);
#endif
}



void takeTime(int event_num){
  struct timespec temp;
  clock_gettime(CLOCK_MONOTONIC, &temp);
#ifdef USAGE_CHECK
  if(event_num>=cur_timers.nevents){
    die("(takeTime)event out of range: %d/%d\n", event_num, cur_timers.nevents);
  }
  
  if(cur_timers.events[event_num].is_bounded &&
     cur_timers.events[event_num].ntimers <= cur_timers.events[event_num].index){
    
    die("Not enough timers allocated: %d/%d\n",
	cur_timers.events[event_num].index,
	cur_timers.events[event_num].ntimers);
  }
#endif
#ifdef high_verbose
    fprintf(stderr, "event: %d/%d taking time: %d/%d\n",
	    event_num, cur_timers.nevents,
	    cur_timers.events[event_num].index, cur_timers.events[event_num].ntimers);
#endif
    int index = cur_timers.events[event_num].index;
    setTimespecAtIndex(cur_timers.events[event_num], index, temp);
    cur_timers.events[event_num].index++;
}


struct timespec getTimeN(int event_num, int n){
#ifdef USAGE_CHECK
  if(event_num>=cur_timers.nevents){
    die("(getTimeN)event out of range: %d/%d\n", event_num, cur_timers.nevents);
  }
  if(n>=cur_timers.events[event_num].index){
    die("That time hasnt been taken yet: %d/%d\n", n, cur_timers.events[event_num].index);
  }
#endif
  return getTimespecAtIndex(cur_timers.events[event_num], n);
}



void diffPattern1(int event_num){
#ifdef USAGE_CHECK
  if(event_num>=cur_timers.nevents){
    die("(diffPattern1)event out of range: %d/%d\n", event_num, cur_timers.nevents);
  }
  if(cur_timers.events[event_num].index == 0){
    die("No Times\n");
  }
#endif
  cur_timers.events[event_num].ndifs = 1;
  cur_timers.events[event_num].difs = (nanoseconds**)mycalloc(1, sizeof(nanoseconds*));
  cur_timers.events[event_num].difs[0] = (nanoseconds*)mycalloc(cur_timers.events[event_num].index,
							 sizeof(nanoseconds));
  for(int i=0;i<cur_timers.events[event_num].index;i++){
    cur_timers.events[event_num].difs[0][i] =
      to_nsecs(getTimespecAtIndex(cur_timers.events[event_num],
				  i+1));
  }
}


void diffPatternN(int event_num, int n){
#ifdef USAGE_CHECK
  if(event_num>=cur_timers.nevents){
    die("(diffPatternN)event out of range: %d/%d\n", event_num, cur_timers.nevents);
  }  
  if((cur_timers.events[event_num].index%n)!=0 || cur_timers.events[event_num].index == 0){
    die("Times taken don't match diff pattern!\n");
  }
#endif

  if(n==1){
    return diffPattern1(event_num);
  }
  cur_timers.events[event_num].ndifs = n-1;
  cur_timers.events[event_num].difs = (nanoseconds**)mycalloc(n-1, sizeof(nanoseconds*));
  int len = cur_timers.events[event_num].index;
  for(int i=0;i<n-1;i++){

    cur_timers.events[event_num].difs[i] = (nanoseconds*)mycalloc(len/n,
								  sizeof(nanoseconds));
  }
  for(int i=0;i<len;i+=n){
    for(int j=i;j<i+(n-1);j++){
      cur_timers.events[event_num].difs[j-i][i/n] =
      ns_diff(getTimespecAtIndex(cur_timers.events[event_num],j+1),
	      getTimespecAtIndex(cur_timers.events[event_num],j));
    }
  }
}

void printEvent(int csv_flag, int csv_header, enum time_unit ptype, int event_num){
  
#ifdef USAGE_CHECK
  if(event_num>=cur_timers.nevents){
    die("event out of range: %d/%d\n", event_num, cur_timers.nevents);
  }
  if(!cur_timers.events[event_num].ndifs){
    die("No times to print!\n");
  }
#endif
  const char* units = unit_to_str(ptype);
  if(csv_flag){
    if(csv_header){
      fprintf(cur_timers.outfile, "EVENT_HEADER,DIF_HEADER,N,UNITS,MIN,MAX,MEAN,MEDIAN,SD,VAR\n");
    }
    for(int i=0;i<cur_timers.events[event_num].ndifs;i++){
      if(cur_timers.ev_headers && cur_timers.ev_headers[event_num]){
	fprintf(cur_timers.outfile,"%s,", cur_timers.ev_headers[event_num]);
      }else{
	fprintf(cur_timers.outfile,"%d,", event_num);
      }
      if(cur_timers.events[event_num].headers &&
	 cur_timers.events[event_num].headers[i]){
	fprintf(cur_timers.outfile,"%s,", cur_timers.events[event_num].headers[i]);
      }else{
	fprintf(cur_timers.outfile,"%d,", i);
      }

      int len = cur_timers.events[event_num].index/(cur_timers.events[event_num].ndifs+1);
      fprintf(cur_timers.outfile, "%d,%s,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf\n",
	      len,
	      units,
	      unit_convert(getMin(cur_timers.events[event_num].difs[i], len), ptype),
	      unit_convert(getMax(cur_timers.events[event_num].difs[i], len), ptype),
	      unit_convert(getMean(cur_timers.events[event_num].difs[i], len), ptype),
	      unit_convert(getMedian(cur_timers.events[event_num].difs[i], len), ptype),
	      unit_convert(getSD(cur_timers.events[event_num].difs[i], len), ptype),
	      unit_convert(getVar(cur_timers.events[event_num].difs[i], len), ptype));
    }
  }
  else{
    for(int i=0;i<cur_timers.events[event_num].ndifs;i++){

      if(cur_timers.ev_headers && cur_timers.ev_headers[event_num]){
	fprintf(cur_timers.outfile,"%s -> {\n", cur_timers.ev_headers[event_num]);
      }else{
	fprintf(cur_timers.outfile,"\tHeader: %d\n,", event_num);
      }
      if(cur_timers.events[event_num].headers &&
	 cur_timers.events[event_num].headers[i]){
	fprintf(cur_timers.outfile,"\tHeader: %s\n", cur_timers.events[event_num].headers[i]);
      }else{
	fprintf(cur_timers.outfile,"\tHeader: %d\n,", i);
      }
      
      int len = cur_timers.events[event_num].index/(cur_timers.events[event_num].ndifs+1);
      fprintf(cur_timers.outfile, "\tN     : %d\n", len);
      fprintf(cur_timers.outfile, "\tMin   : %.3lf %s\n",
	      unit_convert(getMin(cur_timers.events[event_num].difs[i], len), ptype),
	      units);
      fprintf(cur_timers.outfile, "\tMax   : %.3lf %s\n",
	      unit_convert(getMax(cur_timers.events[event_num].difs[i], len), ptype),
	      units);
      fprintf(cur_timers.outfile, "\tMean  : %.3lf %s\n",
	      unit_convert(getMean(cur_timers.events[event_num].difs[i], len), ptype),
	      units);
      fprintf(cur_timers.outfile, "\tMed   : %.3lf %s\n",
	      unit_convert(getMedian(cur_timers.events[event_num].difs[i], len), ptype),
	      units);
      fprintf(cur_timers.outfile, "\tSD    : %.3lf %s\n",
	      unit_convert(getSD(cur_timers.events[event_num].difs[i], len), ptype),
	      units);
      fprintf(cur_timers.outfile, "\tVar   : %.3lf %s\n",
	      unit_convert(getVar(cur_timers.events[event_num].difs[i], len), ptype),
	      units);
#ifdef high_verbose
	fprintf(stderr, "\tData  : [");
	for(int d=0;d<len-1;d++){
	  fprintf(stderr, "%lu,", cur_timers.events[event_num].difs[i][d]);
	}

	fprintf(stderr, "%lu]\n", cur_timers.events[event_num].difs[i][len-1]);
#endif
      fprintf(cur_timers.outfile,"\t}\n\n");
    }
  }
}

void printStats(int csv_flag, enum time_unit ptype){
  for(int i=0;i<cur_timers.nevents;i++){
    printEvent(csv_flag, !i, ptype, i);
  }
}

void freeTiming(){
  for(int i=0;i<cur_timers.nevents;i++){
    tevent cur_event = cur_timers.events[i];
    if(cur_event.difs){
      for(int d=0;d<cur_event.ndifs;d++){
	myFree(cur_event.difs[d]);
      }
      myFree(cur_event.difs);
    }
    if(cur_event.headers){
      myFree(cur_event.headers);
    }
    myFree(cur_event.times);
    for(int t = 0; t<MAX_TIME_ARRAYS; t++){
      myFree(cur_event.times_nobound[t]);
    }
    myFree(cur_event.times_nobound);
  }
  myFree(cur_timers.ev_headers);
  myFree(cur_timers.events);
  if(cur_timers.outfile!=stdout){
    fclose(cur_timers.outfile);
  }
}

void timingBarrierInit(pthread_barrier_t* barrier, int nthreads){
    mybarrierinit(barrier, nthreads);
}

void timingBarrierWait(pthread_barrier_t* barrier, int event_num, int tid){
  pthread_barrier_wait(barrier);
  if(!tid){
    takeTime(event_num);
  }
}

struct timespec getTimespecAtIndex(tevent ev, int index){

  if(!ev.is_bounded){
    int x = fl1_asm(index>>(DEFAULT_MIN_LOG_MINUS_ONE));
    int y = index & ((~(1<<(x+DEFAULT_MIN_LOG_MINUS_ONE+(x==0)))));
    #ifdef USAGE_CHECK
      if(x >= MAX_TIME_ARRAYS || y >= ((1<<(x+(x==0)))<<DEFAULT_MIN_LOG_MINUS_ONE)){
	die("Setting out of bounds: %d -> (%d, %d/%d)\n",
	      index,
	      x,
	      y,
	      (1<<(x+(x==0)))<<DEFAULT_MIN_LOG_MINUS_ONE);
      }
#endif
    return ev.times_nobound[x][y];
  }
  else{
    return ev.times[index];
  }
}

void setTimespecAtIndex(tevent ev, int index, struct timespec t){
    if(!ev.is_bounded){
      int x = fl1_asm(index>>(DEFAULT_MIN_LOG_MINUS_ONE));
      int y = index & ((~(1<<(x+DEFAULT_MIN_LOG_MINUS_ONE+(x==0)))));
#ifdef USAGE_CHECK
      if(x >= MAX_TIME_ARRAYS || y >= ((1<<(x+(x==0)))<<DEFAULT_MIN_LOG_MINUS_ONE)){
	die("Setting out of bounds: %d -> (%d, %d/%d)\n",
	      index,
	      x,
	      y,
	      (1<<(x+(x==0)))<<DEFAULT_MIN_LOG_MINUS_ONE);
      }
#endif
#ifdef high_verbose
      fprintf(stderr,
	      "%d -> (%d, %d/%d)\n",
	      index,
	      x,
	      y,
	      (1<<(x+(x==0)))<<DEFAULT_MIN_LOG_MINUS_ONE);
	      
#endif
      if(!ev.times_nobound[x]){
	ev.times_nobound[x] =
	  (struct timespec *)mycalloc((1<<(x+(x==0)))<<DEFAULT_MIN_LOG_MINUS_ONE,
				      sizeof(struct timespec));
      }
      ev.times_nobound[x][y] = t;
    }
    else{
      ev.times[index] = t;
    }
}
