import { AngularFireAuth } from '@angular/fire/auth';
import { ActivatedRoute, Router } from '@angular/router';
import { RoutePath } from '../../shared/enums/route-path.enum';
import { Component, OnDestroy } from '@angular/core';
import { AngularFirestore } from '@angular/fire/firestore';
import { BehaviorSubject, of, Subject } from 'rxjs';
import { map, switchMap, takeUntil } from 'rxjs/operators';
import { FormControl, FormGroup } from '@angular/forms';
import { PwaService } from '../../services/pwa-service.service';
import { Rating } from '../../models/rating.model';

@Component({
  selector: 'tp-profile',
  templateUrl: './profile.component.html',
  styleUrls: ['./profile.component.scss']
})
export class ProfileComponent implements OnDestroy {
  private unsubscribe$ = new Subject<void>();
  currentId: BehaviorSubject<string> = new BehaviorSubject<string>(null);
  currentUser: any;
  myAccount = false;
  showAddress = false;
  avgRating = 0;
  profileForm;
  ratings: Rating[] = [];
  photoUrl: string;

  searchingUpdates = new Subject<boolean>();
  searchingUpdates$ = this.searchingUpdates.asObservable();

  validServiceWorker = new Subject<boolean>();
  validServiceWorker$ = this.validServiceWorker.asObservable();

  constructor(private readonly route: ActivatedRoute,
              private readonly db: AngularFirestore,
              private readonly afAuth: AngularFireAuth,
              private readonly router: Router,
              public readonly pwaService: PwaService) {
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
    this.pwaService.checkForUpdate().then(
      () => this.validServiceWorker.next(true),
      () => this.validServiceWorker.next(false)
    );

    this.route.queryParams.pipe(
      switchMap(params => {
        this.showAddress = params['show'] === 'true';
        if (params['id']) {
          return of(params['id']);
        } else {
          this.myAccount = true;
          return this.afAuth.user.pipe(map(user => user.uid));
        }
      }),
      switchMap(id =>
        this.db.doc('users/' + id).valueChanges()
      )
    ).subscribe(user => {
      this.currentUser = user;
      this.photoUrl = this.getPhotoUrl();
      this.db.collection<Rating>('ratings', ref => {
        return ref
          .where('ratedTo', '==', this.currentUser.uid);
      }).valueChanges().pipe(takeUntil(this.unsubscribe$.asObservable()),
      ).subscribe(ratings => {
        this.ratings = ratings;
        this.avgRating = ratings.map(item => item.rating).reduce((acc, cur) => acc + cur, 0) / ratings.length;
      });
      this.createForm();
    });

    this.searchingUpdates.next(false);
  }

  createForm(): void {
    this.profileForm = new FormGroup({
      phone: new FormControl(this.currentUser.phone),
      address: new FormControl(this.currentUser.address)
    });
    if (this.showAddress) {
      this.profileForm.get('phone').disable();
      this.profileForm.get('address').disable();
    }
  }

  logout(): void {
    this.afAuth.auth.signOut()
      .then(() => this.router.navigateByUrl(RoutePath.Landing));
  }

  update(): void {
    this.db.doc('users/' + this.currentUser.uid).update({
      address: this.profileForm.get('address').value,
      phone: this.profileForm.get('phone').value
    });
  }

  getPhotoUrl(): string {
    if (!this.currentUser || !this.currentUser.photoURL) {
      return '';
    } else if (this.currentUser.photoURL.includes('google')) {
      return this.currentUser.photoURL + '?sz=150';
    } else if (this.currentUser.photoURL.includes('facebook')) {
      return this.currentUser.photoURL + '?width=150&height=150';

    }
  }

  ngOnDestroy(): void {
    this.unsubscribe$.next();
    this.unsubscribe$.complete();
  }

  checkForUpdates(): void {
    this.searchingUpdates.next(true);
    this.pwaService.checkForUpdate().then(
      () =>
        this.searchingUpdates.next(false),
      err =>
        console.error(err)
    );
  }
}
