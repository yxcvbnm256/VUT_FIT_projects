import { ChangeDetectionStrategy, Component } from '@angular/core';
import { FormBuilder, FormControl } from '@angular/forms';
import { AngularFirestore } from '@angular/fire/firestore';
import { AngularFireAuth } from '@angular/fire/auth';
import { Router } from '@angular/router';
import { RoutePath } from '../../shared/enums/route-path.enum';
import { JobsService } from '../../services/jobs.service';
import { GeolocationService } from '../../services/geolocation.service';
import { CategoriesService } from '../../services/categories.service';
import { Job } from '../../models/job.model';
import { MatSnackBar } from '@angular/material';
import { BehaviorSubject, combineLatest } from 'rxjs';
import { map, startWith } from 'rxjs/operators';


@Component({
  selector: 'tp-jobs',
  templateUrl: './jobs.component.html',
  styleUrls: ['./jobs.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush
})
export class JobsComponent {

  mode = new FormControl('over');
  readonly myJobs$ = this.jobService.getUserJobs;
  readonly appliedJobs$ = this.jobService.getAppliedJobs;

  readonly categoriesList = {};

  group = this.builder.group({
    location: ['Anywhere'],
    category: ['All'],
  });

  locations = ['Anywhere', '5km', '10km', '25km', '50km'];

  current$ = this.geo.getCurrentPosition();
  loc: BehaviorSubject<Position> = new BehaviorSubject(null);
  loc$ = this.loc.asObservable();


  readonly jobs$ = combineLatest(
    this.jobService.getNotMyJobs,
    this.loc$,
    this.group.valueChanges.pipe(startWith({category: 'All', location: 'Anywhere'}))
  ).pipe(
    map(([jobs, geo, form]: [Job[], Position, any]) => {
      let jobF = [...jobs];
      if (form.category === 'All' && form.location === 'Anywhere') {
        return jobs;
      }

      if (form.category !== 'All') {
        jobF = jobF.filter(j => j.category === form.category);
      }

      if (form.location !== 'Anywhere' && geo) {
        const d = Number(form.location.substr(0, form.location.length - 2));
        const latUP = geo.coords.latitude + (d / 6378) * (180 / Math.PI);
        const latDOWN = geo.coords.latitude + (-d / 6378) * (180 / Math.PI);
        const lonRIGHT = geo.coords.longitude + (d / 6378) * (180 / Math.PI) / Math.cos(geo.coords.latitude * Math.PI / 180);
        const lonLEFT = geo.coords.longitude + (-d / 6378) * (180 / Math.PI) / Math.cos(geo.coords.latitude * Math.PI / 180);

        return jobF.filter((job: Job) => job.location.latitude <= latUP && job.location.latitude >= latDOWN && job.location.longitude >= lonLEFT && job.location.longitude <= lonRIGHT);
      }
      return jobF;
    })
  );


  constructor(private db: AngularFirestore,
              private router: Router,
              private auth: AngularFireAuth,
              private readonly jobService: JobsService,
              private readonly geo: GeolocationService,
              private readonly snackBar: MatSnackBar,
              private readonly categories: CategoriesService,
              private builder: FormBuilder) {
    this.current$.subscribe(data => this.loc.next(data));

    this.categoriesList = this.categories.categories;
    this.categoriesList['All'] = 'view_comfy';
  }

  newJobFormSubmitted(data: { status: string; values: Job }): void {
    // skip if user canceled the form
    if (data.status === 'canceled') {
      return;
    }
  }

  goToDetail(id: string): void {
    this.router.navigate([
      RoutePath.Platform,
      RoutePath.Jobs,
      id
    ]);
  }

  get categoriesKeys(): string[] {
    return Object.keys(this.categoriesList);
  }
}
