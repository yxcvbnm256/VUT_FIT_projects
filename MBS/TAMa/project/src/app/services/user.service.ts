import { Injectable } from '@angular/core';
import { AngularFirestore } from '@angular/fire/firestore';
import { AngularFireAuth } from '@angular/fire/auth';
import { UserProfile } from '../models/user-profile';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class UserService {

  constructor(private readonly afAuth: AngularFireAuth,
              private readonly db: AngularFirestore) {
  }

  getCurrentUserProfile(): Observable<UserProfile|undefined> {
    const uid = this.afAuth.auth.currentUser.uid;
    return this.db.doc<UserProfile>('users/' + uid).valueChanges();
  }
}
