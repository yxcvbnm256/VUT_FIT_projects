import { Injectable } from '@angular/core';
import { SwUpdate } from '@angular/service-worker';
import { BeforeInstallPromptEvent } from '../models/before-install-prompt-event';

@Injectable()
export class PwaService {

  promptEvent: BeforeInstallPromptEvent;

  constructor(private readonly swUpdate: SwUpdate) {
    swUpdate.available.subscribe(
      event => {
        if (confirm('Would you like to update an app?')) {
          window.location.reload();
        }
      }
    );

    window.addEventListener(
      'beforeinstallprompt',
      (event: BeforeInstallPromptEvent) => {
        this.promptEvent = event;
      }
    );
  }

  checkForUpdate(): Promise<void> {
    return this.swUpdate.checkForUpdate();
  }
}
