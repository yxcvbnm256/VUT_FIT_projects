import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { ApplicantStatus, Job, JobStatus } from '../models/job.model';
import { AngularFirestore } from '@angular/fire/firestore';
import { map, switchMap } from 'rxjs/operators';
import { AngularFireAuth } from '@angular/fire/auth';
import { UserProfile } from '../models/user-profile';
import { User } from 'firebase';
import { MatSnackBar } from '@angular/material';
import { AngularFireStorage, AngularFireUploadTask } from '@angular/fire/storage';

@Injectable({
  providedIn: 'root'
})
export class JobsService {

  jobsCollection = this.db.collection<Job>('jobs');

  constructor(private readonly db: AngularFirestore,
              private readonly afAuth: AngularFireAuth,
              private readonly afStorage: AngularFireStorage,
              private readonly snackBar: MatSnackBar) {
  }

  get getAllJobs(): Observable<Job[]> {
    return this.jobsCollection.snapshotChanges()
      .pipe(
        map(
          jobs => jobs.map(
            j => {
              const data = j.payload.doc.data() as Job;
              const id = j.payload.doc.id as string;
              return {id, ...data} as Job;
            }
          )
        )
      );
  }

  get getNotMyJobs(): Observable<Job[]> {
    return this.getAllJobs.pipe(
      map((jobs: Job[]) => jobs.filter(
        job =>
          job.userID !== this.afAuth.auth.currentUser.uid &&
          !job.applicants.map(item => item.userId).includes(this.afAuth.auth.currentUser.uid) &&
          job.status === JobStatus.PUBLISHED
      )));
  }

  get getUserJobs(): Observable<Job[]> {
    return this.getAllJobs
      .pipe(
        map(
          jobs =>
            jobs.filter(
              (job: Job) =>
                job.userID === this.afAuth.auth.currentUser.uid
            )
        )
      );
  }

  get getAppliedJobs(): Observable<Job[]> {
    return this.getAllJobs
      .pipe(
        map(
          jobs =>
            jobs.filter(
              (job: Job) => {
                return job.applicants.map(item => item.userId).includes(this.afAuth.auth.currentUser.uid);
              })
        )
      );
  }

  isUsersJob(uuid: string): Observable<boolean> {
    return this.getUserJobs
      .pipe(
        map(
          (jobs: Job[]) =>
            jobs.filter(
              job =>
                job.id === uuid
                && job.userID === this.afAuth.auth.currentUser.uid
            ).length === 1
        )
      );
  }

  getJob(uuid: string): Observable<Job> {
    return this.getAllJobs.pipe(
      map(
        jobs =>
          jobs.find(job => job.id === uuid)
      ),
      switchMap((job: Job) =>
        !job
          ? new Observable<Job>()
          : this.db.collection('users').valueChanges().pipe(
          map(
            (users: UserProfile[]) => {
              const user = users.find(item => item.uid === job.userID);
              return {
                ...job,
                username: user ? user.name : '',
                applicants: job.applicants.map(item => {
                  const uuser = users.find(u => u.uid === item.userId);
                  return {
                    ...item,
                    username: uuser ? uuser.name : ''
                  };
                })
              };
            }
          ))
      )
    );
  }

  deleteJob(uuid: string): Promise<void> {
    return this.jobsCollection.doc(uuid).delete();
  }

  confirmJob(job: Job): void {
    this.db.doc(`jobs/${job.id}`).update({
      status: JobStatus.CONNECTED
    });
  }

  finishJob(job: Job): void {
    this.db.doc(`jobs/${job.id}`).update({
      status: JobStatus.DONE
    });
  }

  applyForJob(job: Job): void {
    this.db.doc(`jobs/${job.id}`).update({
      applicants: [...job.applicants, {
        userId: this.afAuth.auth.currentUser.uid,
        status: ApplicantStatus.UNKNOWN
      }]
    });
  }

  dicardJobApplication(job: Job): void {
    this.db.doc(`jobs/${job.id}`).update({
      applicants: job.applicants.filter(item => item.userId !== this.afAuth.auth.currentUser.uid)
    });
  }

  confirm(job: Job, userId: string): void {
    this.db.doc(`jobs/${job.id}`).update({
      applicants: job.applicants.map(item => item.userId === userId ?
        {userId: item.userId, status: ApplicantStatus.CORFIRMED} :
        {userId: item.userId, status: item.status})
    });
  }

  reject(job: Job, userId: string): void {
    this.db.doc(`jobs/${job.id}`).update({
      applicants: job.applicants.map(item => item.userId === userId ?
        {userId: item.userId, status: ApplicantStatus.REJECTED} :
        {userId: item.userId, status: item.status})
    });
  }

  insertNewJob(newJob: Partial<Job>, images: File[]): void {
    this.afAuth.authState.subscribe(
      (u: User) => {
        const job: Partial<Job> = {
          ...newJob,
          userID: u.uid,
          status: JobStatus.PUBLISHED,
        };
        this.db.collection('jobs').add(job)
          .then(
            (b) => {
              const a = images.map((file, i) => this.afStorage.upload(`image-${b.id}-${i}.${file.type.split('/')[1]}`, file));
              Promise.all(a).then((c) => {
                this.db.doc(`jobs/${b.id}`).update({images: c.map(f => f.metadata.fullPath)}).then(
                  () =>  this.snackBar.open('Job was posted successfully', 'Dismiss', {
                    duration: 2500,
                  })
                );
              });
              return;
            },
            err =>
              console.error(err)
          );
      });
  }
}
