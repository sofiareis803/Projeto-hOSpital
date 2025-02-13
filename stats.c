/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#include <stdio.h>
#include <stdlib.h>

#include <stats.h>
#include <hosptime.h>

const char* statsFileName;
FILE* fpStats;

int openStatsFile(){
    fpStats = fopen(statsFileName, "w");
    return fpStats == NULL ? 1 : 0;
}

int closeStatsFile(){
    return fclose(fpStats);
}

void writeStats(struct data_container* data, int totalAdmissions) {
    fprintf(fpStats, "Process Statistics:\n");
    writeProcessStats(data);
    fprintf(fpStats, "\nAdmission Statistics:\n");
    for (int i = 0; i < totalAdmissions; i++) {
        writeAdmissionStats(data->results[i]);
    }
}

void writeProcessStats(struct data_container* data) {
    for (int i = 0; i < data->n_patients; i++) {
        fprintf(fpStats, "   Patient %d requested %d admissions!\n", i, data->patient_stats[i]);
    }
    for (int i = 0; i < data->n_receptionists; i++) {
        fprintf(fpStats, "   Receptionist %d forwarded %d admissions!\n", i, data->receptionist_stats[i]);
    }
    for (int i = 0; i < data->n_doctors; i++) {
        fprintf(fpStats, "   Doctor %d processed %d admissions!\n", i, data->doctor_stats[i]);
    }
}

// Função auxiliar para obter a diferença em segundos
// entre o create_time e o doctor_time
long getSecDiff(struct admission ad){
    long docSec    = ad.doctor_time.tv_sec;
    long createSec = ad.create_time.tv_sec;

    if(docSec - createSec > 0){
        if(ad.doctor_time.tv_nsec > ad.create_time.tv_nsec)
            return docSec - createSec;
        else 
            return docSec - createSec - 1;
    } 
    return 0;
}

// Função auxiliar para obter a diferença em microssegundos
// entre o create_time e o doctor_time
long getMicroDiff(struct admission ad){
    long docNano    = ad.doctor_time.tv_nsec;
    long createNano = ad.create_time.tv_nsec;

    if(docNano > createNano)
        return (docNano - createNano)/1000;
    else 
        return (docNano + (1000000000 - createNano))/1000;
}

void writeAdmissionStats(struct admission ad) {
    fprintf(fpStats, "Admission: %d\n"
                     "Status: %c\n"
                     "Patient id: %d\n"
                     "Receptionist id: %d\n"
                     "Doctor id: %d\n", 
                     ad.id, ad.status, ad.receiving_patient, 
                     ad.receiving_receptionist, ad.receiving_doctor);

    if (ad.status == 'x' || ad.status == '\0' ){ 
        fprintf(fpStats, "\n"); 
        return;
    }

    char *timestampCreated = getTimeStamp(ad.create_time);
    char *timestampPatient = getTimeStamp(ad.patient_time);
    char *timestampReceptionist = getTimeStamp(ad.receptionist_time);
    char *timestampDoctor = getTimeStamp(ad.doctor_time);

    switch (ad.status) {
        case 'M' : fprintf(fpStats, "Start time: %s\n\n", 
                                    timestampCreated); 
                                    break;

        case 'P' : fprintf(fpStats, "Start time: %s\n"
                                    "Patient time: %s\n\n", 
                                    timestampCreated, 
                                    timestampPatient); 
                                    break;    
                                       
        case 'R' : fprintf(fpStats, "Start time: %s\n"
                                    "Patient time: %s\n"
                                    "Receptionist time: %s\n\n", 
                                    timestampCreated, 
                                    timestampPatient, 
                                    timestampReceptionist); 
                                    break;
        
        default  : fprintf(fpStats, "Start time: %s\n" 
                                    "Patient time: %s\n" 
                                    "Receptionist time: %s\n"
                                    "Doctor time: %s\n"
                                    "Total time: %03ld.%06ld\n\n", 
                                     timestampCreated, 
                                     timestampPatient, 
                                     timestampReceptionist, 
                                     timestampDoctor, 
                                     getSecDiff(ad), 
                                     getMicroDiff(ad)); 
                                     break;
    }

    free(timestampCreated);
    free(timestampPatient);
    free(timestampReceptionist);
    free(timestampDoctor);

}

void execute_stats(struct data_container* data, int totalAdmissions) {
    statsFileName = data->statistics_filename;
    if (openStatsFile() == 1) {
        perror("[Stats] - There was an error acessing/creating the file!");
        exit(1);
    }
    writeStats(data, totalAdmissions);
    if (closeStatsFile() == 1) {
        perror("[Stats] - There was an error closing the file!");
        exit(2);
    }
}
