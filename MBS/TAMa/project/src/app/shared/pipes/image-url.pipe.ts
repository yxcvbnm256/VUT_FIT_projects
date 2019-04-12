import { Pipe, PipeTransform } from '@angular/core';
import { AngularFireStorage } from '@angular/fire/storage';
import { Observable } from 'rxjs';
import { filter, map, startWith } from 'rxjs/operators';
import { DomSanitizer } from '@angular/platform-browser';

@Pipe({
  name: 'imageUrl'
})
export class ImageUrlPipe implements PipeTransform {

  constructor(private afStorage: AngularFireStorage,
              private sanitizer: DomSanitizer) {}

  public transform(value: string): Observable<any> {
    return this.afStorage.ref(value).getDownloadURL().pipe(
      startWith(''),
      filter(item => item != null),
      map(item => this.sanitizer.bypassSecurityTrustStyle(`url('${item}')`)));
  }
}
