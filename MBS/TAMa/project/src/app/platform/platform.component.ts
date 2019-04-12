import { Component, OnDestroy } from '@angular/core';
import { AngularFireMessaging } from '@angular/fire/messaging';
import { combineLatest, filter, flatMap, map, merge, mergeMap, mergeMapTo, takeUntil, tap } from 'rxjs/operators';
import { Observable, Subject } from 'rxjs';
import { AngularFirestore } from '@angular/fire/firestore';
import { AngularFireAuth } from '@angular/fire/auth';
import { UserInfo } from 'firebase';
import { NavigationEnd, Router } from '@angular/router';
import { RoutePath } from '../shared/enums/route-path.enum';
import { GeolocationService } from '../services/geolocation.service';
import { split } from 'ts-node';

@Component({
  selector: 'tp-platform',
  templateUrl: './platform.component.html',
  styleUrls: ['./platform.component.scss'],
})
export class PlatformComponent implements OnDestroy {
  private unsubscribe$ = new Subject<void>();
  isProfile = false;
  RoutePath = RoutePath;
  visible = true;

  ngOnDestroy(): void {
    this.unsubscribe$.next();
    this.unsubscribe$.complete();
  }

  constructor(private router: Router,
              private msgs: AngularFireMessaging,
              private db: AngularFirestore,
              private auth: AngularFireAuth,
              private geo: GeolocationService) {
    this.auth.authState.pipe(
      combineLatest(this.requestPermission()),
      filter(state => state[0] != null),
    ).subscribe(state => {
      const user: UserInfo = state[0];
      this.db
        .doc(`/users/${user.uid}`)
        .update({
          notificationToken: state[1],
        })
        .then((info) => {
          console.log('Updated user info:', info);
        })
        .catch((err) => {
          console.log('Error occured during notification token update:', err);
        });
    });

    this.router.events.pipe(
      filter(event => event instanceof NavigationEnd),
      takeUntil(this.unsubscribe$.asObservable()),
      map((event: NavigationEnd) => event.url.includes(RoutePath.Profile)),
    ).subscribe(result => this.isProfile = result);

    this.router.events.pipe(
      filter(event => event instanceof NavigationEnd),
      takeUntil(this.unsubscribe$.asObservable()),
      map((event: NavigationEnd) => {
        const splitted = event.url.split('/');
        return splitted.length > 3 || event.url.includes(`${RoutePath.Profile}?`) || event.url.includes(RoutePath.Rating);
      })
    ).subscribe(data => this.visible = data);
  }

  requestPermission(): Observable<any> {
    return this.msgs.requestPermission
      .pipe(
        mergeMapTo(this.msgs.tokenChanges)
      );
  }

  goBack(): void{
    window.history.back();
  }
}
