#include <stdio.h>
/*.......*/        /* συμπερίληψη αρχείων */
/*.......*/        /* ορισμοί-μακροεντολές */
/*.......*/        /* δηλώσεις */
#define NQ 5       /* αριθμός σταθμών */
#define NJ         /* αριθμός εργασιών (πελατών) στο δίκτυο */
#define EVENTLIMIT /* μέγιστος αριθμός γεγονότων \
(συνθήκη τερματισμού) */
typedef struct JobElement *Jobptr;
typedef struct EventElement *Eventptr;
struct EventElement
{                      /* στοιχείο της λίστας γεγονότων */
    float time;        /* χρονική στιγμή */
    Jobptr job;        /* εργασία που σχετίζεται με το γεγονός */
    Eventptr next;     /* επόμενο/προηγούμενο γεγονός */
    Eventptr previous; /* (διπλά συνδεδεμένη λίστα) */
};
struct JobElement
{                     /* εργασία */
    int currentQueue; /* τρέχων σταθμός (αρίθμηση 0/*....... */
    float request;    /* (υπολειπόμενος) χρόνος εξυπηρέτησης */
    Jobptr nextJob;   /* επόμενη εργασία στη λίστα του σταθμού */
    Eventptr event;   /* δρομολογημένο γεγονός
      που σχετίζεται με την εργασία */
};
int i;
Eventptr firstEvent, lastEvent; /* λίστα γεγονότων */
float clock;                    /* ρολόι */
int endCycle;                   /* έλεγχος τέλους αναγεννητικού κύκλου */
struct Queue
{                        /* σταθμός */
    int discipline;      /* κανονισμός εξυπηρέτησης
         1:FCFS,2:PS,3:IS,4:LCFSPR */
    int numberServers;   /* αριθμός εξυπηρετητών:
      >=1 για FCFS, 1 για PS και LCFSPR,
      NJ (μέγιστη τιμή) για IS */
    float meanService;   /* μέσος χρόνος εξυπηρέτησης,
      υποθέτουμε εκθετική κατανομή */
    float routing[NQ];   /* πιθανότητες δρομολόγησης */
    Jobptr firstInQueue; /* λίστα εργασιών στον σταθμό */
    Jobptr lastInQueue;
    int length; /* αριθμός εργασιών στον σταθμό
    float oldClock; /* χρονική στιγμή τελευταίου γεγονότος στον σταθμό */
    /* αθροίσματα μετρήσεων σε κάθε κύκλο */
    float sumTimeLength;   /* άθροισμα εμβαδών ορθογωνίων:
      χρόνος x αριθμός εργασιών */
    float sumBusyTime;     /* άθροισμα διαστημάτων απασχόλησης */
    int numberCompletions; /* αριθμός αναχωρήσεων */
    /* αθροίσματα μετρήσεων για ολους τους κύκλους */
    float bt;                     /* busyTime */
    float tl;                     /* timeLength */
    float nc;                     /* numberCompletions */
    float btsq;                   /* busyTime squared */
    float btxcl;                  /* busyTime x cycleLength */
    float ncsq;                   /* numberCompletions squared */
    float ncxcl;                  /* numberCompletions x cycleLength */
    float tlsq;                   /* timeLength squared */
    float tlxcl;                  /* timeLength x cycleLength */
    float tlxnc;                  /* timeLength x numberCompletions */
    float util, tput, ql, qt;     /* μέσες τιμές δεικτών */
    float dutil, dtput, dql, dqt; /* διαστήματα εμπιστοσύνης */
    float varbt, varnc, vartl;
    float covarbtcl, covarnccl, covartlcl, covartlnc;
};
struct Queue queues[NQ];
int numberEvents, numberCycles, nocycm1;
float timeCycleStarted, cycleLength; /* διάρκεια αναγεννητικού κύκλου */
float sumcl, sumclsq, varcl, dcl;
Jobptr tempJob;
/*------------------------------------------------------------------*/
void insertEvent(float, Jobptr);
/* εισαγωγή γεγονότος στη λίστα γεγονότων
ορίσματα: χρόνος, εργασία */
void removeEvent(Eventptr, float *, Jobptr *);
/* εξαγωγή γεγονότος από τη λίστα γεγονότων
επιστρέφει τον χρόνο και την εργασία του γεγονότος. */
/*------------------------------------------------------------------*/
void complete(Jobptr j)
{
    /* τέλος εξυπηρέτησης της εργασίας j. */
    int leng;
    Jobptr l;
    float t;
    struct Queue *q;
    q = &queues[j->currentQueue];
    /* στατιστικά στοιχεία */
    q->numberCompletions = q->numberCompletions + 1;
    q->sumTimeLength = q->sumTimeLength + (clock - q->oldClock) * q->length;
    q->sumBusyTime = q->sumBusyTime + (clock - q->oldClock) *
                                          min(q->length, q->numberServers);
    q->oldClock = clock;
    /* ενημέρωση μεταβλητών */
    q->length = q->length - 1;
    if ((q->discipline == 1) /* FCFS */ || (q->length == 0))
    {
        q->firstInQueue = q->firstInQueue->nextJob;
        if (q->firstInQueue == NULL)
            q->lastInQueue = NULL;
        if (q->length >= q->numberServers)
        {
            leng = 1;
            l = q->firstInQueue;
            while (leng < q->numberServers)
            {
                l = l->nextJob;
                leng = leng + 1;
            }
            l->request = -q->meanService * log(random());
            insertEvent(clock + l->request, l);
        }
    }
    else if (discipline == 2) /* PS */
    {
        t = j->request;
        q->firstInQueue = q->firstInQueue->nextJob;
        l = q->firstInQueue;
        while (l != NULL)
        {
            l->request = l->request - t;
            l = l->nextJob;
        }
        insertEvent(clock + q->firstInQueue->request * q->length,
                    q->firstInQueue);
    }
    else if (discipline == 3) /* IS */
    {
        t = j->request;
        q->firstInQueue = q->firstInQueue->nextJob;
        l = q->firstInQueue;
        while (l != NULL)
        {
            l->request = l->request - t;
            l = l->nextJob;
        }
    }
    else /* discipline=LCFSPR */
    {
        q->firstInQueue = q->firstInQueue->nextJob;
        insertEvent(clock + q->firstInQueue->request,
                    q->firstInQueue);
    }
} /* complete */
/*------------------------------------------------------------------*/
void updateQueue(int i, Jobptr j)
{
    /* εισάγει την εργασία j στην ουρά i,
    στην κατάλληλη θέση σύμφωνα με το j->request.
    ουρές τύπου PS και IS θεωρούνται διατεταγμένες.
    υποτίθεται ότι η ουρά δεν είναι άδεια. */
    Jobptr temp, firstInQueue;
    struct Queue *q;
    q = &queues[i];
    if (j->request < q->firstInQueue->request)
    {
        j->nextJob = q->firstInQueue;
        firstInQueue = j;
    }
    else if (j->request >= q->lastInQueue->request)
    {
        q->lastInQueue->nextJob = j;
        j->nextJob = NULL;
        q->lastInQueue = j;
    }
    else
    {
        temp = q->firstInQueue;
        while (j->request >= temp->nextjob->request)
        {
            temp = temp->nextjob;
            j->nextjob = temp->nextJob;
            temp->nextJob = j;
        }
    } /* updateQueue*/
}
/*------------------------------------------------------------------*/
void arrive(Jobptr j, int c)
{
    /* άφιξη της εργασίας j στην ουρά c. */
    float t;
    Jobptr dummyJob, temp;
    struct Queue *q;
    j->currentqueue = c;
    q = &queues[c];
    /* στατιστικά στοιχεία */
    q->sumTimeLength = q->sumTimeLength + (clock - q->oldClock) * q->length;
    q->sumBusyTime = q->sumBusyTime + (clock - q->oldClock) *
                                          min(q->length, q->numberServers);
    q->oldClock = clock;
    /* ενημέρωση μεταβλητών */
    if ((discipline == 1) /* FCFS */ || (firstInQueue == NULL))
    {
        j->nextJob = NULL;
        if (q->firstInQueue == NULL)
            q->firstInQueue = j;
        else
            q->lastInQueue->nextJob = j;
        q->lastInQueue = j;
        q->length = q->length + 1;
        if (q->length <= q->numberServers)
        {
            j->request = -q->meanService * log(random());
            insertEvent(clock + j->request, j)
        }
    }
    else if (discipline == 2) /* PS */
    {
        removeEvent(q->firstInQueue->event, t, dummyJob);
        t = q->firstInQueue->request - (t - clock) / q->length;
        temp = firstInQueue;
        while (temp != NULL)
        {
            temp->request = temp->request - t;
            temp = temp->nextJob;
        }
        j->request = -q->meanService * log(random());
        updateQueue(c, j);
        q->length = q->length + 1;
        insertEvent(clock + q->firstInQueue->request * q->length,
                    q->firstInQueue);
    }
    else if (discipline == 3) /* IS */
    {
        j->request = -q->meanService * log(random());
        updateQueue(c, j);
        q->length = q->length + 1;
        insertEvent(clock + j->request, j);
    }
    else /* discipline=LCFSPR */
    {
        removeEvent(q->firstInQueue->event, t, dummyjob);
        q->firstInQueue->request = t - clock; /* διακοπή εξυπηρέτησης */
        j->nextjob = q->firstInQueue;
        q->firstInQueue = j;
        q->length = q->length + 1;
        j->request = -q->meanService * log(random());
        insertEvent(clock + j->request, j);
    }
} /* arrive */
/*------------------------------------------------------------------*/
int nextNode(Jobptr j)
{
    /* εύρεση του επόμενου σταθμού που επισκέπτεται η εργασία j. */
    float prob;
    int i;
    struct Queue *q;
    q = &queues[j->currentQueue];
    prob = random();
    i = 1;
    while ((prob > q->routing[i]) && (i != NQ))
    {
        prob = prob - q->routing[i];
        i = i + 1;
    }
    return i;
} /* nextnode */
/*------------------------------------------------------------------*/
void checkCycle()
{
    /* ΄Ελεγχος για τέλος αναγεννητικού κύκλου. Ενημέρωση συσσωρευτών. */
    int i;
    struct Queue *q;
    endCycle = 0;
    if ((1 == 1 /*.......*/) /* συνθήκη αναγεννητικής κατάστασης */
        && (numberEvents > 0))
    {
        endCycle = 1;
        numberCycles = numberCycles + 1;
        cycleLength = clock - timeCycleStarted;
        timeCycleStarted = clock;
        sumcl = sumcl + cycleLength;
        sumclsq = sumclsq + sqr(cycleLength);
        for (i = 0; i < NQ; i++)
        {
            q = &queues[i];
            q->sumTimeLength = q->sumTimeLength + (clock - q->oldClock) * q->length;
            q->sumBusyTime = (q->sumBusyTime + (clock - q->oldClock) *
                                                   min(q->length, q->numberServers)) /
                             q->numberServers;
            q->oldClock = clock;
            q->bt = q->bt + q->sumBusyTime;
            q->tl = q->tl + q->sumTimeLength;
            q->nc = q->nc + q->numberCompletions;
            q->btsq = q->btsq + sqr(q->sumBusyTime);
            q->btxcl = q->btxcl + q->sumBusyTime * cycleLength;
            q->sumBusyTime = 0.0;
            q->ncsq = q->ncsq + sqr(q->numberCompletions);
            q->ncxcl = q->ncxcl + q->numberCompletions * cycleLength;
            q->tlsq = q->tlsq + sqr(q->sumTimeLength);
            q->tlxcl = q->tlxcl + q->sumTimeLength * cycleLength;
            q->tlxnc = q->tlxnc + q->sumTimeLength * q->numberCompletions;
            q->numberCompletions = 0;
            q->sumTimeLength = 0.0;
        }
    }
} /* checkCycle */
/*------------------------------------------------------------------*/
main()
{
    /* κύριο πρόγραμμα */

    /* αρχικοποίηση γεννήτριας τυχαίων αριθμών */

    /* αρχικοποιήσεις */
    numberEvents = 0;
    firstEvent = NULL;
    lastEvent = NULL;
    clock = 0.0;
    numberCycles = 0;
    endCycle = 0;
    timeCycleStarted = 0.0;
    sumcl = 0.0;
    sumclsq = 0.0;
    struct Queue *q;
    /* παράμετροι σταθμών */
    q = &queues[0];
    q->discipline =.q->numberServers =.q->meanService =.for (i = 0; i < NQ; i++) scanf("%f", &routing[i]);
    /*.......*/ /*.......*/
    /*.......*/ /*.......*/
    q = &queues[NQ - 1];
    q->discipline =.q->numberServers =.q->meanservice =.for (i = 0; i < NQ; i++) scanf("%f", &routing[i]);
    for (i = 0; i < NQ; i++)
    {
        q = &queues[i];
        q->firstInQueue = NULL;
        q->lastInQueue = NULL;
        q->length = 0;
        q->oldClock = 0.0;
        q->sumTimeLength = 0.0;
        q->sumBusyTime = 0.0;
        q->numberCompletions = 0;
        q->bt = 0.0;
        q->tl = 0.0;
        q->nc = 0.0;
        q->btsq = 0.0;
        q->btxcl = 0.0;
        q->ncsq = 0.0;
        q->ncxcl = 0.0;
        q->tlsq = 0.0;
        q->tlxcl = 0.0;
        q->tlxnc = 0.0
    }
    /* αρχικοποίηση σε αναγεννητική κατάσταση:
    δημιουργία και δρομολόγηση NJ εργασιών */
    tempJob = (Jobptr)malloc(sizeof JobElement);
    arrive(tempJob, /*.......*/ .);
    /*.......*/ /*.......*/
    /*.......*/ /*.......*/
    tempJob = (Jobptr)malloc(sizeof JobElement);
    arrive(tempJob, /*.......*/ .);
    /* προσομοίωση */
    while ((firstEvent != NULL) &&
           ((numberEvents < EVENTLIMIT) || (!endCycle)))
    {
        /* η προσομοίωση σταματά σε τέλος αναγεννητικού κύκλου,
        όταν έχουν συμπληρωθεί τουλάχιστον EVENTLIMIT γεγονότα. */
        removeEvent(firstEvent, clock, tempJob);
        numberEvents = numberEvents + 1;
        complete(tempJob);
        i = nextNode(tempJob);
        arrive(tempJob, i);
        checkCycle;
    }
    /* στατιστικά αποτελέσματα */
    printf("\n");
    printf("number of events: %f\n simulated time: %f\n",
           numberEvents, clock);
    printf("\n");
    printf("queue utilization throughput queue-length queueing-time\n");
    /* υποθέτουμε ότι numberCycles>1 */
    cycleLength = sumcl / numberCycles;
    nocycm1 = numberCycles - 1;
    varcl = (sumclsq - sqr(sumcl) / numberCycles) / nocycm1;
    /* βαθμός εμπιστοσύνης 1-α=0.9 */
    for (i = 0; i < NQ; i++)
    {
        q = &queues[i];
        if (q->nc > 0.0)
        {
            q->util = q->bt / sumcl;
            q->varbt = (q->btsq - sqr(q->bt) / numberCycles) / nocycm1;
            q->covarbtcl = (q->btxcl - q->bt * sumcl / numberCycles) / nocycm1;
            q->dutil = 1.645 * sqrt((q->varbt - 2 * q->util * q->covarbtcl + sqr(q->util) * varcl) / numberCycles) / cycleLength;
            q->tput = q->nc / sumcl;
            q->varnc = (q->ncsq - sqr(q->nc) / numberCycles) / nocycm1;
            q->covarnccl = (q->ncxcl - q->nc * sumcl / numberCycles) / nocycm1;
            q->dtput = 1.645 * sqrt((q->varnc - 2 * q->tput * q->covarnccl + sqr(q->tput) * varcl) / numberCycles) / cycleLength;
            q->ql = q->tl / sumcl;
            q->vartl = (q->tlsq - sqr(q->tl) / numberCycles) / nocycm1;
            q->covartlcl = (q->tlxcl - q->tl * sumcl / numberCycles) / nocycm1;
            q->dql = 1.645 * sqrt((q->vartl - 2 * q->ql * q->covartlcl + sqr(q->ql) * varcl) / numberCycles) / cycleLength;
            q->qt = q->tl / q->nc;
            q->covartlnc = (q->tlxnc - q->tl * q->nc / numberCycles) / nocycm1;
            q->dqt = 1.645 * sqrt((q->vartl - 2 * q->qt * q->covartlnc + sqr(q->qt) * q->varnc) / numberCycles) / (nc / numberCycles);
            printf("%d\n", i);
            printf("lower %f %f %f %f \n", q->util - q->dutil, q->tput - q->dtput,
                   q->ql - q->dql, q->qt - q->dqt);
            printf("mean %f %f %f %f \n", q->util, q->tput,
                   q->ql, q->qt);
            printf("upper %f %f %f %f \n",
                   q->util + q->dutil, q->tput + q->dtput,
                   q->ql + q->dql, q->qt + q->dqt);
            printf("\n");
        }
        printf("n");
        printf("number of cycles: %d\n", numberCycles);
        printf("average number of events: %f\n", numberEvents / numberCycles);
        dcl = 1.645 * sqrt(varcl / numberCycles);
        printf("average cycle length: %f, c.i.: ( %f , %f)",
               cycleLength, cycleLength - dcl, cycleLength + dcl);
        return 0;
    }
}