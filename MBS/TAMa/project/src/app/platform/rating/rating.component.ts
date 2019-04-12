import { AngularFireAuth } from '@angular/fire/auth';
import { ActivatedRoute, Router } from '@angular/router';
import { RoutePath } from '../../shared/enums/route-path.enum';
import { Component, OnDestroy } from '@angular/core';
import { AngularFirestore } from '@angular/fire/firestore';
import { BehaviorSubject, Subject } from 'rxjs';
import { FormControl, FormGroup, Validators } from '@angular/forms';
import {PwaService} from '../../services/pwa-service.service';
import { map, takeUntil } from 'rxjs/operators';
import { Job } from '../../models/job.model';
import { Rating } from '../../models/rating.model';

@Component({
  selector: 'tp-rating',
  templateUrl: './rating.component.html',
  styleUrls: ['./rating.component.scss']
})
export class RatingComponent implements OnDestroy {
  private unsubscribe$ = new Subject<void>();
  currentId: string;
  currentName: string;
  ratingForm;
  jobId: string;
  jobName: string;
  userId: string;
  userName: string;
  rating: Rating;
  rated: boolean;
  ratingsCollection = this.db.collection<Rating>('rating');

  validServiceWorker = new Subject<boolean>();
  validServiceWorker$ = this.validServiceWorker.asObservable();

  constructor(private readonly route: ActivatedRoute,
              private readonly db: AngularFirestore,
              private readonly afAuth: AngularFireAuth,
              private readonly router: Router,
              public readonly pwaService: PwaService) {
    this.jobId = this.route.snapshot.queryParams['jobId'];
    this.userId = this.route.snapshot.queryParams['userId'];
    this.jobName = this.route.snapshot.queryParams['jobName'];
    this.userName = this.route.snapshot.queryParams['userName'];

    this.afAuth.user.pipe(
      takeUntil(this.unsubscribe$.asObservable()),
    ).subscribe(user => {
      this.currentId = user.uid;
      this.currentName = user.displayName;
      this.db.collection<Rating>('ratings', ref => {
        return ref
          .where('ratedBy', '==', this.currentId)
          .where('jobId', '==', this.jobId);
      }).valueChanges().pipe(takeUntil(this.unsubscribe$.asObservable()),
      ).subscribe(ratings => {
        this.rating = ratings[0];
        this.rated = !!this.rating;
        this.createForm();
      });


    });

    /*console.log(this.route.snapshot.queryParams);
    if (this.route.snapshot.queryParams['id']) {
      this.currentId.next(this.route.snapshot.params['id']);
    } else {
      this.afAuth.user.pipe(
        takeUntil(this.unsubscribe$.asObservable()),
      ).subscribe(user => this.currentId.next(user.uid));
    }
    this.currentId.pipe(
      takeUntil(this.unsubscribe$.asObservable()),
    ).subscribe(id => {
      this.db.doc('users/' + id).valueChanges().pipe(
        takeUntil(this.unsubscribe$.asObservable()),
      ).subscribe(result => {this.currentUser = result; console.log(result);});
    });*/
  }

  createForm(): void {
    this.ratingForm = new FormGroup({
      jobId: new FormControl(this.jobId),
      text: new FormControl({value: this.rated ? this.rating.text : '', disabled: this.rated} , [Validators.required]),
      rating: new FormControl({value: this.rated ? this.rating.rating : '', disabled: this.rated}, [Validators.required]),
      ratedBy: new FormControl(this.currentId),
      ratedTo: new FormControl(this.userId),
      name: new FormControl(this.currentName)
    });
  }


  update(): void {
    this.db.collection('ratings/').add(this.ratingForm.value);
  }

  ngOnDestroy(): void {
    this.unsubscribe$.next();
    this.unsubscribe$.complete();
  }
}
