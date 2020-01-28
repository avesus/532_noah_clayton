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

double unit_convert(double time_ns, time_unit desired){
  double conversion = (((double)ns_per_sec)/(double)desired);
  return time_ns/conversion;
}

const char* unit_to_str(time_unit u){
  int index = 0;
  unsigned long s = u;
  while(s/unit_change){
    s = s/unit_change;
    index++;
  }
  return time_unit_str[index];
}

void printTimeHR(struct timespec t1, struct timespec t2){
  printTimeHR(ns_diff(t1, t2));
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
  fprintf(cur_timers.outfile, "%.3lf %s\n", exec_time, unit_to_str(u));
}


struct timespec grabTime(){
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return t;
}

void initTiming(int* ntimers, int nevents, char*** dif_headers, char** ev_headers){
  return initTiming(ntimers, nevents, NULL, NULL);
}

void initTiming(int* ntimers, int nevents, char*** dif_headers, char** ev_headers, FILE* outfile){
#ifdef USAGE_CHECK
  if(!ntimers || !nevents){
    die("ntimers or nevents not specified: %p:%d\n", ntimers, nevents);
  }
#endif
  cur_timers.outfile = stdout;
  if(outfile){
    cur_timers.outfile = outfile;
  }

  cur_timers.ntimes = ntimers;
  cur_timers.dif_headers = (const char***)dif_headers;
  cur_timers.ev_headers = (const char**)ev_headers;
  cur_timers.ndifs = (int*)mycalloc(nevents, sizeof(int));
  cur_timers.difs = (nanoseconds***)mycalloc(nevents, sizeof(nanoseconds**));
  cur_timers.index = (int*)mycalloc(nevents, sizeof(int));
  cur_timers.times = (struct timespec**)mycalloc(nevents, sizeof(struct timespec*));
  for(int i=0;i<nevents;i++){
    if(!ntimers[i]){
      die("ntimers[%d] must be > 0\n");
    }
    cur_timers.times[i] = (struct timespec*)mycalloc(ntimers[i], sizeof(struct timespec));
  }
}


void takeTime(){
#ifdef USAGE_CHECK
  if(cur_timers.ntimes <= cur_timers.index){
    die("Not enough timers allocated: %d/%d\n", cur_timers.index, cur_timers.ntimes);
  }
#endif
  takeTime(0);
}

void takeTime(int event_num){
#ifdef USAGE_CHECK
  if(event_num>=cur_timers.nevents){
    die("event out of range: %d/%d\n", event_num, cur_timers.nevents);
  }
  if(cur_timers.ntimes[event_num] <= cur_timers.index[event_num]){
    die("Not enough timers allocated: %d/%d\n", cur_timers.index, cur_timers.ntimes);
  }
#endif
  clock_gettime(CLOCK_MONOTONIC, &(cur_timers.times[event_num][cur_timers.index[event_num]]));
  cur_timers.index[event_num]++;
}

struct timespec getTimeN(int n){
  return getTimeN(0, n);
}
struct timespec getTimeN(int event_num, int n){
#ifdef USAGE_CHECK
  if(event_num>=cur_timers.nevents){
    die("event out of range: %d/%d\n", event_num, cur_timers.nevents);
  }
  if(n>=cur_timers.index[event_num]){
    die("That time hasnt been taken yet: %d/%d\n", n, cur_timers.index);
  }
#endif
  return cur_timers.times[event_num][n];
}


void diffPattern1(){
  return diffPattern1(0);
}

void diffPattern1(int event_num){
#ifdef USAGE_CHECK
  if(event_num>=cur_timers.nevents){
    die("event out of range: %d/%d\n", event_num, cur_timers.nevents);
  }
  if(cur_timers.index == 0){
    die("No Times\n");
  }
#endif
  cur_timers.ndifs[event_num] = 1;
  cur_timers.difs[event_num] = (nanoseconds**)mycalloc(1, sizeof(nanoseconds*));
  cur_timers.difs[event_num][0] = (nanoseconds*)mycalloc(cur_timers.index[event_num],
							 sizeof(nanoseconds));
  for(int i=0;i<cur_timers.index[event_num];i++){
    cur_timers.difs[event_num][0][i] = to_nsecs(cur_timers.times[event_num][i+1]);
  }
}

void diffPatternN(int n){
  return diffPatternN(0, n);
}

void diffPatternN(int event_num, int n){
#ifdef USAGE_CHECK
  if(event_num>=cur_timers.nevents){
    die("event out of range: %d/%d\n", event_num, cur_timers.nevents);
  }  
  if((cur_timers.index[event_num]%n)!=0 || cur_timers.index[event_num] == 0){
    die("Times taken don't match diff pattern!\n");
  }
#endif

  if(n==1){
    return diffPattern1(event_num);
  }
  cur_timers.ndifs[event_num] = n-1;
  cur_timers.difs[event_num] = (nanoseconds**)mycalloc(n-1, sizeof(nanoseconds*));
  for(int i=0;i<n-1;i++){
    cur_timers.difs[event_num][i] = (nanoseconds*)mycalloc(cur_timers.index[event_num]/n,
							   sizeof(nanoseconds));
  }
  for(int i=0;i<cur_timers.index[event_num];i+=n){
    for(int j=i;j<i+(n-1);j++){
      cur_timers.difs[event_num][j-i][i] = ns_diff(cur_timers.times[event_num][j+1],
						   cur_timers.times[event_num][j]);
    }
  }
}

void printEvent(int csv_flag, int csv_header, time_unit ptype, int event_num){
  
#ifdef USAGE_CHECK
  if(event_num>=cur_timers.nevents){
    die("event out of range: %d/%d\n", event_num, cur_timers.nevents);
  }
  if(!cur_timers.ndifs[event_num]){
    die("No times to print!\n");
  }
#endif
  const char* units = unit_to_str(ptype);
  if(csv_flag){
    if(csv_header){
      fprintf(cur_timers.outfile, "EV_HEADER,DIF_HEADER,N,UNITS,MIN,MAX,MEAN,MEDIAN,SD,VAR\n");
    }
    for(int i=0;i<cur_timers.ndifs[event_num];i++){
      if(cur_timers.ev_headers && cur_timers.ev_headers[event_num]){
	fprintf(cur_timers.outfile,"%s,", cur_timers.ev_headers[event_num]);
      }else{
	fprintf(cur_timers.outfile,"%d,", event_num);
      }
      if(cur_timers.dif_headers &&
	 cur_timers.dif_headers[event_num] &&
	 cur_timers.dif_headers[event_num][i]){
	fprintf(cur_timers.outfile,"%s,", cur_timers.dif_headers[event_num][i]);
      }else{
	fprintf(cur_timers.outfile,"%d,", i);
      }

      int len = cur_timers.index[event_num]/cur_timers.ndifs[event_num];
      fprintf(cur_timers.outfile, "%d,%s,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf\n",
	      len,
	      units,
	      unit_convert(getMin(cur_timers.difs[event_num][i], len), ptype),
	      unit_convert(getMax(cur_timers.difs[event_num][i], len), ptype),
	      unit_convert(getMean(cur_timers.difs[event_num][i], len), ptype),
	      unit_convert(getMedian(cur_timers.difs[event_num][i], len), ptype),
	      unit_convert(getSD(cur_timers.difs[event_num][i], len), ptype),
	      unit_convert(getVar(cur_timers.difs[event_num][i], len), ptype));
    }
  }
  else{
    for(int i=0;i<cur_timers.ndifs[event_num];i++){

      if(cur_timers.ev_headers && cur_timers.ev_headers[event_num]){
	fprintf(cur_timers.outfile,"%s -> {\n", cur_timers.ev_headers[event_num]);
      }else{
	fprintf(cur_timers.outfile,"\tHeader: %d\n,", event_num);
      }
      if(cur_timers.dif_headers &&
	 cur_timers.dif_headers[event_num] &&
	 cur_timers.dif_headers[event_num][i]){
	fprintf(cur_timers.outfile,"\tHeader: %s\n", cur_timers.dif_headers[event_num][i]);
      }else{
	fprintf(cur_timers.outfile,"\tHeader: %d\n,", i);
      }

      int len = cur_timers.index[event_num]/cur_timers.ndifs[event_num];
      fprintf(cur_timers.outfile, "\tN     : %d\n", len);
      fprintf(cur_timers.outfile, "\tMin   : %.3lf %s\n",
	      unit_convert(getMin(cur_timers.difs[event_num][i], len), ptype),
	      units);
      fprintf(cur_timers.outfile, "\tMax   : %.3lf %s\n",
	      unit_convert(getMax(cur_timers.difs[event_num][i], len), ptype),
	      units);
      fprintf(cur_timers.outfile, "\tMean  : %.3lf %s\n",
	      unit_convert(getMean(cur_timers.difs[event_num][i], len), ptype),
	      units);
      fprintf(cur_timers.outfile, "\tMed   : %.3lf %s\n",
	      unit_convert(getMedian(cur_timers.difs[event_num][i], len), ptype),
	      units);
      fprintf(cur_timers.outfile, "\tSD    : %.3lf %s\n",
	      unit_convert(getSD(cur_timers.difs[event_num][i], len), ptype),
	      units);
      fprintf(cur_timers.outfile, "\tVar   : %.3lf %s\n",
	      unit_convert(getVar(cur_timers.difs[event_num][i], len), ptype),
	      units);
      fprintf(cur_timers.outfile,"\t}\n\n");
    }
  }
}

void printStats(int csv_flag, time_unit ptype){
  for(int i=0;i<cur_timers.nevents;i++){
    printEvent(csv_flag, !i, ptype, i);
  }
}
