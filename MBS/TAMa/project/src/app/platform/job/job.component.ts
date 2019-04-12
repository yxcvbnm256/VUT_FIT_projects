import { ChangeDetectionStrategy, ChangeDetectorRef, Component, OnDestroy } from '@angular/core';
import { map, switchMap, take, takeUntil } from 'rxjs/operators';
import { ActivatedRoute, Router } from '@angular/router';
import { AngularFirestore } from '@angular/fire/firestore';
import { combineLatest, Subject } from 'rxjs';
import { RoutePath } from '../../shared/enums/route-path.enum';
import { JobsService } from '../../services/jobs.service';
import { MatSnackBar } from '@angular/material';
import { ApplicantStatus, Job, JobRelation, JobStatus } from '../../models/job.model';
import { AngularFireAuth } from '@angular/fire/auth';

@Component({
  selector: 'tp-job',
  templateUrl: './job.component.html',
  styleUrls: ['./job.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush
})
export class JobComponent implements OnDestroy {

  private readonly unsubscribe$ = new Subject<void>();

  job$ = this.route.params.pipe(
    takeUntil(this.unsubscribe$.asObservable()),
    switchMap(
      (params: any) =>
        this.jobService.getJob(params['id'])
    )
  );

  jobRelation$ = this.job$.pipe(
    map((job: Job) => {
      if (job.userID === this.afAuth.auth.currentUser.uid) {
        return JobRelation.CREATED;
      }
      const applicant = job.applicants.find(item => item.userId === this.afAuth.auth.currentUser.uid);
      if (!applicant) {
        return JobRelation.UNKNOWN;
      }
      return JobRelation.APPLIED;
    }));

  canDelete$ = combineLatest([this.job$, this.jobRelation$]).pipe(
    map(([job, relation]: [Job, JobRelation]) => job.status !== JobStatus.DONE && relation === JobRelation.CREATED)
  );

  canApply$ = combineLatest([this.job$, this.jobRelation$]).pipe(
    map(([job, relation]: [Job, JobRelation]) => job.status === JobStatus.PUBLISHED && relation === JobRelation.UNKNOWN)
  );

  jobApplied$ = combineLatest([this.job$, this.jobRelation$]).pipe(
    map(([job, relation]: [Job, JobRelation]) => job.status === JobStatus.PUBLISHED && relation === JobRelation.APPLIED)
  );

  jobConfirm$ = combineLatest([this.job$, this.jobRelation$]).pipe(
    map(([job, relation]: [Job, JobRelation]) => relation === JobRelation.CREATED && job.status === JobStatus.PUBLISHED)
  );

  jobDone$ = combineLatest([this.job$, this.jobRelation$]).pipe(
    map(([job, relation]: [Job, JobRelation]) => relation === JobRelation.CREATED && job.status === JobStatus.CONNECTED)
  );

  applicants$ = this.job$.pipe(
    map(item => item.status === JobStatus.PUBLISHED ?
      item.applicants :
      item.applicants.filter(user => user.status === ApplicantStatus.CORFIRMED)));

  applicationAccepted$ = this.job$.pipe(
    map((job: Job) =>
      job.status === JobStatus.CONNECTED && job.applicants.some(item =>
      item.userId === this.afAuth.auth.currentUser.uid && item.status === ApplicantStatus.CORFIRMED
      )
    )
  );

  applicationRejected$ = this.job$.pipe(
    map((job: Job) =>
      job.status === JobStatus.CONNECTED && job.applicants.some(item =>
      item.userId === this.afAuth.auth.currentUser.uid && item.status === ApplicantStatus.REJECTED
      )
    )
  );

  rate$ = this.job$.pipe(
    map((job: Job) =>
      job.status === JobStatus.DONE && job.applicants.some(item =>
      item.userId === this.afAuth.auth.currentUser.uid && item.status === ApplicantStatus.CORFIRMED
      )
    )
  );

  showAddress$ = combineLatest([this.applicationAccepted$, this.rate$, this.jobDone$]).pipe(
    map(([application, rate, done]) => application || rate || done)
  );

  shift = 0;

  tabIndex = 0;
  galleryVisible = false;

  ApplicantStatus = ApplicantStatus;
  JobRelation = JobRelation;
  JobStatus = JobStatus;

  constructor(private route: ActivatedRoute,
              private router: Router,
              private detR: ChangeDetectorRef,
              private db: AngularFirestore,
              private readonly afAuth: AngularFireAuth,
              private readonly jobService: JobsService,
              private readonly snackBar: MatSnackBar) {
  }

  ngOnDestroy(): void {
    this.unsubscribe$.next();
    this.unsubscribe$.complete();
  }

  selectUser(id: string, visible: boolean = false): void {
    if (id === this.afAuth.auth.currentUser.uid) {
      this.router.navigate(
        [
          RoutePath.Platform,
          RoutePath.Profile,
        ]);
    } else {
      this.showAddress$.pipe(
        take(1)).subscribe((show) =>
        this.router.navigate(
          [
            RoutePath.Platform,
            RoutePath.Profile,
          ],
          {
            queryParams: {
              id: id,
              show: show
            }
          }));
    }
  }

  deleteJob(): void {
    this.route.params.subscribe(
      params =>
        this.router.navigate([
          RoutePath.Platform,
          RoutePath.Jobs
        ]).then(
          () =>
            this.jobService.deleteJob(params['id'])
              .then(
                () =>
                  this.snackBar.open('Job was deleted successfully', 'Dismiss', {
                    duration: 2500,
                  }),
                err =>
                  this.snackBar.open('Unable to delete job', 'Dismiss', {
                    duration: 2500,
                  })
              )
        )
    );
  }

  toLeft(images: string[]): void {
    this.shift = this.shift + 1 === images.length ? this.shift : this.shift + 1;
  }

  toRight(): void {
    this.shift = this.shift - 1 < 0 ? this.shift : this.shift - 1;
  }

  setShift(index: number): void {
    this.shift = index;
  }

  applyForJob(): void {
    this.job$.pipe(take(1)).subscribe((job: Job) => this.jobService.applyForJob(job));
  }

  confirm(userId: string): void {
    this.job$.pipe(take(1)).subscribe((job: Job) => this.jobService.confirm(job, userId));
  }

  reject(userId: string): void {
    this.job$.pipe(take(1)).subscribe((job: Job) => this.jobService.reject(job, userId));
  }

  discardJob(): void {
    this.job$.pipe(take(1)).subscribe((job: Job) => this.jobService.dicardJobApplication(job));
  }

  confirmJob(): void {
    this.job$.pipe(take(1)).subscribe((job: Job) => this.jobService.confirmJob(job));
  }

  finishJob(): void {
    this.job$.pipe(take(1)).subscribe((job: Job) => this.jobService.finishJob(job));
  }

  rateUser(job: Job, userId: string, userName: string): void {
    this.router.navigate(
      [RoutePath.Platform, RoutePath.Rating],
      {queryParams: {userId: userId, jobId: job.id, jobName: job.name, userName: userName}});
  }

}
