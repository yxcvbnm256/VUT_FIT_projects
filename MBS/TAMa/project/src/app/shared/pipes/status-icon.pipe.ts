import { Pipe, PipeTransform } from '@angular/core';
import { ApplicantStatus, Job, JobStatus } from '../../models/job.model';
import { AngularFireAuth } from '@angular/fire/auth';

@Pipe({
  name: 'statusIcon'
})
export class StatusIconPipe implements PipeTransform {

  constructor(private readonly afAuth: AngularFireAuth) {
  }

  public transform(job: Job, type: string): string {
    if (type === 'applications') {
      if (job.status === JobStatus.PUBLISHED) {
        return 'help_outline';
      } else if (job.status === JobStatus.CONNECTED && job.applicants.some(item => item.status === ApplicantStatus.CORFIRMED && item.userId === this.afAuth.auth.currentUser.uid)) {
        return 'done';
      } else if (job.status === JobStatus.DONE && job.applicants.some(item => item.status === ApplicantStatus.CORFIRMED && item.userId === this.afAuth.auth.currentUser.uid)) {
        return 'done_all';
      } else {
        return 'clear';
      }
    }

    if (type === 'offers') {
      switch (job.status) {
        case JobStatus.PUBLISHED:
          return 'help_outline';
        case JobStatus.CONNECTED:
          return 'done';
        case JobStatus.DONE:
          return 'done_all';
      }
    }
    return '';

  }
}
