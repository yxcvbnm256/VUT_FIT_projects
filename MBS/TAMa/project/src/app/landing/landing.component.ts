import { Component, OnInit } from '@angular/core';
import { AngularFireAuth } from '@angular/fire/auth';
import { auth } from 'firebase/app';
import UserCredential = auth.UserCredential;
import {ReplaySubject, Subject} from 'rxjs';
import { Router } from '@angular/router';
import { RoutePath } from '../shared/enums/route-path.enum';
import { PwaService } from '../services/pwa-service.service';
import { FormBuilder, Validators } from '@angular/forms';
import {state} from '@angular/animations';

enum FormState {
  Login,
  Register,
}

@Component({
  selector: 'tp-landing',
  templateUrl: './landing.component.html',
  styleUrls: ['./landing.component.scss']
})
export class LandingComponent implements OnInit {

  loginGroup = this.builder.group({
    login: ['', Validators.required],
    password: ['', Validators.required],
  });

  registerGroup = this.builder.group({
    email: ['', Validators.required],
    password: ['', Validators.required],
  });

  private readonly failedToSubmit = new Subject<boolean>();
  readonly failedToSubmit$ = this.failedToSubmit.asObservable();

  private readonly formState = new ReplaySubject<FormState>();
  readonly formState$ = this.formState.asObservable();

  readonly FormState = FormState;

  constructor(public afAuth: AngularFireAuth,
              private readonly router: Router,
              public pwa: PwaService,
              private readonly builder: FormBuilder) {
    this.formState.next(this.FormState.Login);
    this.formState$.subscribe(
      value => {
        this.failedToSubmit.next(false);
        this.loginGroup.reset();
        this.registerGroup.reset();
      }
    );
  }

  loginWithGoogle(): void {
    this.login(new auth.GoogleAuthProvider())
      .catch(error => {
        console.error(error);
      });
  }

  loginWithFacebook(): void {
    this.login(new auth.FacebookAuthProvider())
      .catch(error => {
        console.error(error);
      });
  }

  login(authProvider: auth.AuthProvider): Promise<void> {
    return this.afAuth.auth
      .signInWithPopup(authProvider)
      .then((creds: UserCredential) => {
        this.router.navigateByUrl(RoutePath.Platform);
      });
  }

  loginWithPassword(): void {
    this.afAuth.auth.signInWithEmailAndPassword(
      this.loginGroup.controls.login.value,
      this.loginGroup.controls.password.value
    ).then(
      () =>
        this.router.navigateByUrl(RoutePath.Platform)
    ).catch(
      () =>
        this.failedToSubmit.next(true)
    );
  }

  registerWithPassword(): void {
    this.afAuth.auth.createUserWithEmailAndPassword(
      this.registerGroup.controls.email.value,
      this.registerGroup.controls.password.value
    ).then(
      (creds: UserCredential) =>
        this.router.navigateByUrl(RoutePath.Platform)
    ).catch(
      () =>
        this.failedToSubmit.next(true)
    );
  }

  switchToLogin(): void {
    this.formState.next(FormState.Login);
  }

  switchToRegister(): void {
    this.formState.next(FormState.Register);
  }

  ngOnInit(): void {
    //
  }

  installPwa(): void {
    this.pwa.promptEvent.prompt();
  }
}
